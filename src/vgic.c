#include "aarch64.h"
#include "gic.h"
#include "vgic.h"
#include "log.h"
#include "param.h"
#include "vcpu.h"
#include "mmio.h"

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

static struct vgic_irq *vgic_irq(struct vcpu *vcpu, int intid) {
  return NULL;
}

int vgicd_mmio_read(struct vcpu *vcpu, int r, u64 offset, enum mmio_accsize accsize) {
  int intid;
  struct vgic_irq *irq;
  struct vgic_cpu *vgic = vcpu->vgic;

  printf("mmio_read %p", offset);
  switch(offset) {
    case GICD_IPRIORITYR(0) ... GICD_IPRIORITYR(254)+3:
      intid = (offset - GICD_IPRIORITYR(0)) / 4;
      printf("%d,\n", intid);
      break;
  }

  return 0;
}

int vgicd_mmio_write(struct vcpu *vcpu, int r, u64 offset, enum mmio_accsize accsize) {
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

  return vgic;
}

struct vgic_cpu *new_vgic_cpu() {
  struct vgic_cpu *vgic = allocvgic_cpu();

  for(int i = 0; i < gic_lr_max; i++)
    vgic->used_lr[i] = 0;

  return vgic;
}

