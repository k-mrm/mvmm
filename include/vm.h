#ifndef MVMM_VM_H
#define MVMM_VM_H

#include "types.h"
#include "param.h"
#include "vgic.h"
#include "spinlock.h"

struct mmio_access;
struct mmio_info;
struct vcpu;

struct vm {
  char name[16];
  int nvcpu;
  struct vcpu *vcpus[VCPU_MAX];
  u64 *vttbr;
  struct vgic *vgic;
  struct mmio_info *pmap;
  int npmap;
  int used;
  spinlock_t lock;
};

extern struct vm vms[VM_MAX];

void new_vm(char *name, int ncpu, u64 img_start, u64 img_size, u64 entry, u64 allocated);

void pagetrap(struct vm *vm, u64 va, u64 size,
              int (*read_handler)(struct vcpu *, u64, u64 *, struct mmio_access *),
              int (*write_handler)(struct vcpu *, u64, u64, struct mmio_access *));
#endif
