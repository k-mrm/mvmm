#ifndef MVMM_GIC_H
#define MVMM_GIC_H

#include "types.h"

void gic_init(void);
u32 gic_read_iar(void);
void gic_eoi(u32 iar, int grp);
void gic_deactive_int(u32 irq);

u64 gic_read_lr(int n);
void gic_write_lr(int n, u64 val);
u64 gic_make_lr(u32 pirq, u32 virq, int grp);

#endif
