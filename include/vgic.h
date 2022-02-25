#ifndef MVMM_VGIC_H
#define MVMM_VGIC_H

#include "types.h"
#include "param.h"

struct vgic {
  int used;
};

void gic_init(void);
struct vgic *new_vgic(void);
u32 gic_read_irq(void);

#endif
