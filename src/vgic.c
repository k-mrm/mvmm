#include "aarch64.h"
#include "vgic.h"
#include "log.h"

/* gicv3 controller */

#define ich_hcr_el2   sysreg32(4, c12, c11, 0)
#define ich_vtr_el2   sysreg32(4, c12, c11, 1)
#define ich_vmcr_el2  sysreg32(4, c12, c11, 7)
#define ich_lr0_el2   sysreg32(4, c12, c12, 0)
#define ich_lr1_el2   sysreg32(4, c12, c12, 1)
#define ich_lr2_el2   sysreg32(4, c12, c12, 2)
#define ich_lr3_el2   sysreg32(4, c12, c12, 3)
#define ich_lr4_el2   sysreg32(4, c12, c12, 4)
#define ich_lr5_el2   sysreg32(4, c12, c12, 5)
#define ich_lr6_el2   sysreg32(4, c12, c12, 6)
#define ich_lr7_el2   sysreg32(4, c12, c12, 7)
#define ich_lr8_el2   sysreg32(4, c12, c13, 0)
#define ich_lr9_el2   sysreg32(4, c12, c13, 1)
#define ich_lr10_el2  sysreg32(4, c12, c13, 2)
#define ich_lr11_el2  sysreg32(4, c12, c13, 3)
#define ich_lr12_el2  sysreg32(4, c12, c13, 4)
#define ich_lr13_el2  sysreg32(4, c12, c13, 5)
#define ich_lr14_el2  sysreg32(4, c12, c13, 6)
#define ich_lr15_el2  sysreg32(4, c12, c13, 7)

#define icc_iar1_el1     sysreg32(0, c12, c12, 0)
#define icc_igrpen0_el1  sysreg32(0, c12, c12, 6)
#define icc_igrpen1_el1  sysreg32(0, c12, c12, 7)
#define icc_pmr_el1      sysreg32(0, c4, c6, 0)

#define ICH_HCR_EN  (1<<0)

#define ICH_VMCR_VENG0  (1<<0)
#define ICH_VMCR_VENG1  (1<<1)

#define ICH_LR_VINTID(n)   ((n) & 0xffffffffL)
#define ICH_LR_PINTID(n)   (((n) & 0x3ffL) << 32)
#define ICH_LR_GROUP(n)    (((n) & 0x1L) << 60)
#define ICH_LR_HW          (1L << 61)
#define ICH_LR_STATE(n)    (((n) & 0x3L) << 62)
#define LR_PENDING   1L
#define LR_ACTIVE    2L

struct vgic vgics[VM_MAX];

static struct vgic *allocvgic() {
  for(struct vgic *vgic = vgics; vgic < &vgics[VM_MAX]; vgic++) {
    if(vgic->used == 0) {
      vgic->used = 1;
      return vgic;
    }
  }

  return NULL;
}

static void gicc_init(void) {
  write_sysreg(icc_igrpen0_el1, 0);
  write_sysreg(icc_igrpen1_el1, 0);

  write_sysreg(icc_pmr_el1, 0xff);

  write_sysreg(icc_igrpen0_el1, 1);
  write_sysreg(icc_igrpen1_el1, 1);

  isb();
}

static void gich_init(void) {
  write_sysreg(ich_vmcr_el2, ICH_VMCR_VENG0|ICH_VMCR_VENG1);
  write_sysreg(ich_hcr_el2, ICH_HCR_EN);

  isb();
}

static u64 read_lr(int n) {
  u64 val;

  switch(n) {
    case 0:   read_sysreg(val, ich_lr0_el2); break;
    case 1:   read_sysreg(val, ich_lr1_el2); break;
    case 2:   read_sysreg(val, ich_lr2_el2); break;
    case 3:   read_sysreg(val, ich_lr3_el2); break;
    case 4:   read_sysreg(val, ich_lr4_el2); break;
    case 5:   read_sysreg(val, ich_lr5_el2); break;
    case 6:   read_sysreg(val, ich_lr6_el2); break;
    case 7:   read_sysreg(val, ich_lr7_el2); break;
    case 8:   read_sysreg(val, ich_lr8_el2); break;
    case 9:   read_sysreg(val, ich_lr9_el2); break;
    case 10:  read_sysreg(val, ich_lr10_el2); break;
    case 11:  read_sysreg(val, ich_lr11_el2); break;
    case 12:  read_sysreg(val, ich_lr12_el2); break;
    case 13:  read_sysreg(val, ich_lr13_el2); break;
    case 14:  read_sysreg(val, ich_lr14_el2); break;
    case 15:  read_sysreg(val, ich_lr15_el2); break;
    default:  panic("?");
  }

  return val;
}

static void write_lr(int n, u64 val) {
  switch(n) {
    case 0:   write_sysreg(ich_lr0_el2, val); break;
    case 1:   write_sysreg(ich_lr1_el2, val); break;
    case 2:   write_sysreg(ich_lr2_el2, val); break;
    case 3:   write_sysreg(ich_lr3_el2, val); break;
    case 4:   write_sysreg(ich_lr4_el2, val); break;
    case 5:   write_sysreg(ich_lr5_el2, val); break;
    case 6:   write_sysreg(ich_lr6_el2, val); break;
    case 7:   write_sysreg(ich_lr7_el2, val); break;
    case 8:   write_sysreg(ich_lr8_el2, val); break;
    case 9:   write_sysreg(ich_lr9_el2, val); break;
    case 10:  write_sysreg(ich_lr10_el2, val); break;
    case 11:  write_sysreg(ich_lr11_el2, val); break;
    case 12:  write_sysreg(ich_lr12_el2, val); break;
    case 13:  write_sysreg(ich_lr13_el2, val); break;
    case 14:  write_sysreg(ich_lr14_el2, val); break;
    case 15:  write_sysreg(ich_lr15_el2, val); break;
    default:  panic("?");
  }
}

static u64 make_lr(u32 pirq, u32 virq, int grp) {
  return ICH_LR_STATE(LR_PENDING) | ICH_LR_HW | ICH_LR_GROUP(grp) | ICH_LR_PINTID(pirq) | ICH_LR_VINTID(virq);
}

void vgic_lr_pending(struct vgic *vgic, u32 pirq, u32 virq, int grp) {
  u64 lr = make_lr(pirq, virq, grp);
  int n = vgic->nlr++;
  printf("pending");
  print64(lr);

  write_lr(n, lr);
}

u32 gic_read_irq() {
  u32 i;
  read_sysreg(i, icc_iar1_el1);
  return i;
}

struct vgic *new_vgic() {
  struct vgic *vgic = allocvgic();

  vgic->nlr = 0;

  return vgic;
}

void gic_init(void) {
  vmm_log("gic init...\n");
  gicc_init();
  gich_init();
}
