#include "aarch64.h"
#include "gic.h"
#include "log.h"
#include "memmap.h"

/* gicv3 controller */

int gic_lr_max = 0;

u64 gic_read_lr(int n) {
  if(gic_lr_max <= n)
    panic("lr");

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

void gic_write_lr(int n, u64 val) {
  if(gic_lr_max <= n)
    panic("lr");

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

u64 gic_make_lr(u32 pirq, u32 virq, int grp) {
  return ICH_LR_STATE(LR_PENDING) | ICH_LR_HW | ICH_LR_GROUP(grp) | ICH_LR_PINTID(pirq) | ICH_LR_VINTID(virq);
}

u32 gic_read_iar() {
  u32 i;
  read_sysreg(i, icc_iar1_el1);
  return i;
}

void gic_eoi(u32 iar, int grp) {
  if(grp == 0)
    write_sysreg(icc_eoir0_el1, iar);
  else if(grp == 1)
    write_sysreg(icc_eoir1_el1, iar);
  else
    panic("?");
}

void gic_deactive_int(u32 irq) {
  write_sysreg(icc_dir_el1, irq);
}

static int gic_max_listregs() {
  u64 i;
  read_sysreg(i, ich_vtr_el2);
  return (i & 0x1f) + 1;
}

int gic_max_spi() {
  u32 typer = gicd_r(GICD_TYPER);
  u32 lines = typer & 0x1f;
  u32 max_spi = 32 * (lines + 1) - 1;

  return max_spi < 1020? max_spi : 1019;
}

static void gicc_init(void) {
  write_sysreg(icc_igrpen0_el1, 0);
  write_sysreg(icc_igrpen1_el1, 0);

  write_sysreg(icc_pmr_el1, 0xff);
  write_sysreg(icc_ctlr_el1, ICC_CTLR_EOImode(1));

  write_sysreg(icc_igrpen0_el1, 1);
  write_sysreg(icc_igrpen1_el1, 1);

  isb();
}

static void gich_init(void) {
  write_sysreg(ich_vmcr_el2, ICH_VMCR_VENG0|ICH_VMCR_VENG1);
  write_sysreg(ich_hcr_el2, ICH_HCR_EN);

  gic_lr_max = gic_max_listregs();

  isb();
}

void gic_init(void) {
  vmm_log("gic init...\n");
  gicc_init();
  gich_init();
}
