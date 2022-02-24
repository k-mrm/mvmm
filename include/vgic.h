#ifndef MVMM_VGIC_H
#define MVMM_VGIC_H

#include "types.h"
#include "param.h"

struct vgic {
  ;
} vgic[VM_MAX];

void gic_init(void);
struct vgic *new_vgic();

#endif
