#ifndef MVMM_VGIC_H
#define MVMM_VGIC_H

#include "types.h"
#include "param.h"

struct vgic {
  int used;
  int nlr;
};

void gic_init(void);
struct vgic *new_vgic(void);
u32 gic_read_irq(void);

void vgic_lr_pending(struct vgic *vgic, u32 pirq, u32 virq, int grp);

#endif
