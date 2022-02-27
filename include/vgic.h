#ifndef MVMM_VGIC_H
#define MVMM_VGIC_H

#include "types.h"
#include "param.h"

struct vgic {
  int used;
  int used_lr[16];
};

void gic_init(void);
struct vgic *new_vgic(void);
u32 gic_read_iar(void);
void gic_eoi(u32 iar, int grp);
void gic_deactive_int(u32 irq);

void gic_irq_enter(struct vgic *vgic);
void vgic_lr_pending(struct vgic *vgic, u32 pirq, u32 virq, int grp);

#endif
