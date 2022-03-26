#ifndef MVMM_VGIC_H
#define MVMM_VGIC_H

#include "types.h"
#include "param.h"
#include "gic.h"

struct vcpu;

struct vgic_irq {
  u8 priority;  /* ipriorityr */
  u8 target;
  u8 enabled: 1;
  u8 igroup: 1;
};

struct vgic {
  int used;
  int spi_max;
  int nspis;
  u32 ctlr;     /* GICD_CTLR */
  struct vgic_irq *spis;
};

/* vgic cpu interface */
struct vgic_cpu {
  int used;
  u16 used_lr;
  struct vgic_irq sgis[GIC_NSGI];
  struct vgic_irq ppis[GIC_NPPI];
};

void vgic_irq_enter(struct vcpu *vcpu);
struct vgic *new_vgic(void);
struct vgic_cpu *new_vgic_cpu(void);
void vgic_forward_virq(struct vcpu *vcpu, u32 pirq, u32 virq, int grp);

#endif
