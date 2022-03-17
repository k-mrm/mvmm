#include "aarch64.h"
#include "gic.h"
#include "vgic.h"
#include "log.h"
#include "param.h"

struct vgic vgics[VM_MAX];

extern int gic_lr_max;

static struct vgic *allocvgic() {
  for(struct vgic *vgic = vgics; vgic < &vgics[VM_MAX]; vgic++) {
    if(vgic->used == 0) {
      vgic->used = 1;
      return vgic;
    }
  }

  return NULL;
}

static int vgic_alloc_lr(struct vgic *vgic) {
  for(int i = 0; i < gic_lr_max; i++) {
    if(vgic->used_lr[i] == 0) {
      vgic->used_lr[i] = 1;
      return i;
    }
  }

  return -1;
}

void vgic_irq_enter(struct vgic *vgic) {
  for(int i = 0; i < gic_lr_max; i++) {
    if(vgic->used_lr[i] == 1) {
      u64 lr = gic_read_lr(i);
      /* already handled by guest */
      if((lr & ICH_LR_STATE(LR_MASK)) == LR_INACTIVE)
        vgic->used_lr[i] = 0;
    }
  }
}

int vgicd_mmio_write(struct vgic *vgic, u64 gpa, u64 x) {
  ;
}

int vgicd_mmio_read(struct vgic *vgic, u64 gpa, u64 *x) {
  ;
}

void vgic_forward_virq(struct vgic *vgic, u32 pirq, u32 virq, int grp) {
  u64 lr = gic_make_lr(pirq, virq, grp);

  int n = vgic_alloc_lr(vgic);
  if(n < 0)
    panic("no lr");

  gic_write_lr(n, lr);
}

struct vgic *new_vgic() {
  struct vgic *vgic = allocvgic();

  for(int i = 0; i < gic_lr_max; i++)
    vgic->used_lr[i] = 0;

  return vgic;
}
