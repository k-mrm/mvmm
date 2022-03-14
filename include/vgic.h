#ifndef MVMM_VGIC_H
#define MVMM_VGIC_H

#include "types.h"
#include "param.h"
#include "gic.h"

struct vgic {
  int used;
  int used_lr[16];
};

void vgic_irq_enter(struct vgic *vgic);
struct vgic *new_vgic(void);
void vgic_forward_irq(struct vgic *vgic, u32 pirq, u32 virq, int grp);

#endif
