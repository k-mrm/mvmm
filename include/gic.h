#ifndef MVMM_GIC_H
#define MVMM_GIC_H

#include "types.h"

#define ich_hcr_el2   arm_sysreg(4, c12, c11, 0)
#define ich_vtr_el2   arm_sysreg(4, c12, c11, 1)
#define ich_vmcr_el2  arm_sysreg(4, c12, c11, 7)
#define ich_lr0_el2   arm_sysreg(4, c12, c12, 0)
#define ich_lr1_el2   arm_sysreg(4, c12, c12, 1)
#define ich_lr2_el2   arm_sysreg(4, c12, c12, 2)
#define ich_lr3_el2   arm_sysreg(4, c12, c12, 3)
#define ich_lr4_el2   arm_sysreg(4, c12, c12, 4)
#define ich_lr5_el2   arm_sysreg(4, c12, c12, 5)
#define ich_lr6_el2   arm_sysreg(4, c12, c12, 6)
#define ich_lr7_el2   arm_sysreg(4, c12, c12, 7)
#define ich_lr8_el2   arm_sysreg(4, c12, c13, 0)
#define ich_lr9_el2   arm_sysreg(4, c12, c13, 1)
#define ich_lr10_el2  arm_sysreg(4, c12, c13, 2)
#define ich_lr11_el2  arm_sysreg(4, c12, c13, 3)
#define ich_lr12_el2  arm_sysreg(4, c12, c13, 4)
#define ich_lr13_el2  arm_sysreg(4, c12, c13, 5)
#define ich_lr14_el2  arm_sysreg(4, c12, c13, 6)
#define ich_lr15_el2  arm_sysreg(4, c12, c13, 7)

#define icc_pmr_el1      arm_sysreg(0, c4, c6, 0)
#define icc_eoir0_el1    arm_sysreg(0, c12, c8, 1)
#define icc_dir_el1      arm_sysreg(0, c12, c11, 1)
#define icc_iar1_el1     arm_sysreg(0, c12, c12, 0)
#define icc_eoir1_el1    arm_sysreg(0, c12, c12, 1)
#define icc_ctlr_el1     arm_sysreg(0, c12, c12, 4)
#define icc_igrpen0_el1  arm_sysreg(0, c12, c12, 6)
#define icc_igrpen1_el1  arm_sysreg(0, c12, c12, 7)

#define ICC_CTLR_EOImode(m) ((m) << 1)

#define ICH_HCR_EN  (1<<0)

#define ICH_VMCR_VENG0  (1<<0)
#define ICH_VMCR_VENG1  (1<<1)

#define ICH_LR_VINTID(n)   ((n) & 0xffffffffL)
#define ICH_LR_PINTID(n)   (((n) & 0x3ffL) << 32)
#define ICH_LR_GROUP(n)    (((n) & 0x1L) << 60)
#define ICH_LR_HW          (1L << 61)
#define ICH_LR_STATE(n)    (((n) & 0x3L) << 62)
#define LR_INACTIVE  0L
#define LR_PENDING   1L
#define LR_ACTIVE    2L
#define LR_MASK      3L


void gic_init(void);
u32 gic_read_iar(void);
void gic_eoi(u32 iar, int grp);
void gic_deactive_int(u32 irq);

u64 gic_read_lr(int n);
void gic_write_lr(int n, u64 val);
u64 gic_make_lr(u32 pirq, u32 virq, int grp);

#endif
