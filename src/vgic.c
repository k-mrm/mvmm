#include "aarch64.h"
#include "gic.h"
#include "vgic.h"
#include "log.h"
#include "param.h"
#include "vcpu.h"
#include "mmio.h"
#include "pmalloc.h"

struct vgic vgics[VM_MAX];
struct vgic_cpu vgic_cpus[VCPU_MAX];

extern int gic_lr_max;

static struct vgic *allocvgic() {
  for(struct vgic *vgic = vgics; vgic < &vgics[VM_MAX]; vgic++) {
    if(vgic->used == 0) {
      vgic->used = 1;
      return vgic;
    }
  }

  vmm_warn("null vgic");
  return NULL;
}

static struct vgic_cpu *allocvgic_cpu() {
  for(struct vgic_cpu *v = vgic_cpus; v < &vgic_cpus[VCPU_MAX]; v++) {
    if(v->used == 0) {
      v->used = 1;
      return v;
    }
  }

  vmm_warn("null vgic_cpu");
  return NULL;
}

static int vgic_cpu_alloc_lr(struct vgic_cpu *vgic) {
  for(int i = 0; i < gic_lr_max; i++) {
    if(vgic->used_lr[i] == 0) {
      vgic->used_lr[i] = 1;
      return i;
    }
  }

  vmm_warn("lr :(");
  return -1;
}

void vgic_irq_enter(struct vcpu *vcpu) {
  struct vgic_cpu *vgic = vcpu->vgic;

  for(int i = 0; i < gic_lr_max; i++) {
    if(vgic->used_lr[i] == 1) {
      u64 lr = gic_read_lr(i);
      /* already handled by guest */
      if((lr & ICH_LR_STATE(LR_MASK)) == LR_INACTIVE)
        vgic->used_lr[i] = 0;
    }
  }
}

static struct vgic_irq *vgic_get_irq(struct vcpu *vcpu, int intid) {
  if(is_ppi(intid))
    return &vcpu->vgic->ppis[intid - 16];
  else if(is_spi(intid))
    return &vcpu->vm->vgic->spis[intid - 32];

  panic("unknown %d", intid);
  return NULL;
}

int vgicd_mmio_read(struct vcpu *vcpu, u64 offset, u64 *val, enum mmio_accsize accsize) {
  int intid, idx;
  struct vgic_irq *irq;
  struct vgic *vgic = vcpu->vm->vgic;

  switch(offset) {
    case GICD_CTLR:
      *val = vgic->ctlr;
      return 0;
    case GICD_TYPER:
      return gicd_r(GICD_TYPER);
    case GICD_ISENABLER(0) ... GICD_ISENABLER(31)+3: {
      u32 iser = 0;

      intid = (offset - GICD_ISENABLER(0)) / sizeof(u32) * 32;
      for(int i = 0; i < 32; i++) {
        irq = vgic_get_irq(vcpu, intid+i);
        iser |= (u32)irq->enabled << i;
      }

      *val = iser;
      return 0;
    }
    case GICD_ICPENDR(0) ... GICD_ICPENDR(31)+3:
      return 0;
    case GICD_IPRIORITYR(0) ... GICD_IPRIORITYR(254)+3: {
      u32 ipr = 0;

      intid = offset - GICD_IPRIORITYR(0);
      for(int i = 0; i < 4; i++) {
        irq = vgic_get_irq(vcpu, intid+i);
        ipr |= (u32)irq->priority << (i * 8);
      }

      *val = ipr;
      return 0;
    }
    case GICD_ITARGETSR(0) ... GICD_ITARGETSR(254)+3: {
      u32 itar = 0;

      intid = offset - GICD_ITARGETSR(0);
      for(int i = 0; i < 4; i++) {
        irq = vgic_get_irq(vcpu, intid+i);
        itar |= (u32)irq->target << (i * 8);
      }

      *val = itar;
      return 0;
    }
  }

  vmm_warn("unhandled %p\n", offset);
  return -1;
}

int vgicd_mmio_write(struct vcpu *vcpu, u64 offset, u64 val, enum mmio_accsize accsize) {
  int intid;
  struct vgic_irq *irq;
  struct vgic *vgic = vcpu->vm->vgic;

  switch(offset) {
    case GICD_CTLR:
      vgic->ctlr = val;
      return 0;
    case GICD_TYPER:
      goto readonly;
    case GICD_ISENABLER(0) ... GICD_ISENABLER(31)+3:
      intid = (offset - GICD_ISENABLER(0)) / sizeof(u32) * 32;
      for(int i = 0; i < 32; i++) {
        irq = vgic_get_irq(vcpu, intid+i);
        irq->enabled = (val >> i) & 0x1;
      }
      return 0;
    case GICD_ICPENDR(0) ... GICD_ICPENDR(31)+3:
      return 0;
    case GICD_IPRIORITYR(0) ... GICD_IPRIORITYR(254)+3:
      intid = (offset - GICD_IPRIORITYR(0)) / sizeof(u32) * 4;
      vmm_log("ipriority offset %p %d %d\n", offset, intid, val);
      for(int i = 0; i < 4; i++) {
        irq = vgic_get_irq(vcpu, intid+i);
        irq->priority = (val >> (i * 8)) & 0xff;
      }
      return 0;
    case GICD_ITARGETSR(0) ... GICD_ITARGETSR(254)+3:
      intid = (offset - GICD_ITARGETSR(0)) / sizeof(u32) * 4;
      for(int i = 0; i < 4; i++) {
        irq = vgic_get_irq(vcpu, intid+i);
        irq->target = (val >> (i * 8)) & 0xff;
      }
      return 0;
  }

  vmm_warn("unhandled %p\n", offset);
  return -1;

readonly:
  return 0;
}

void vgic_forward_virq(struct vcpu *vcpu, u32 pirq, u32 virq, int grp) {
  struct vgic_cpu *vgic = vcpu->vgic;

  u64 lr = gic_make_lr(pirq, virq, grp);

  int n = vgic_cpu_alloc_lr(vgic);
  if(n < 0)
    panic("no lr");

  gic_write_lr(n, lr);
}

struct vgic *new_vgic() {
  struct vgic *vgic = allocvgic();
  vgic->spi_max = gic_max_spi();
  vgic->nspis = vgic->spi_max - 31;
  vgic->ctlr = 0;
  vgic->spis = (struct vgic_irq *)pmalloc();
  vmm_log("nspis %d sizeof nspi %d\n", vgic->nspis, sizeof(struct vgic_irq) * vgic->nspis);

  return vgic;
}

struct vgic_cpu *new_vgic_cpu() {
  struct vgic_cpu *vgic = allocvgic_cpu();

  for(int i = 0; i < gic_lr_max; i++)
    vgic->used_lr[i] = 0;

  return vgic;
}
