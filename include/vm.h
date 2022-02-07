#ifndef MVMM_VM_H
#define MVMM_VM_H

#include "types.h"

#define VM_MAX  2

struct vcpu;

struct vm {
  u64 *vttbr;
  struct vcpu *vcpu;
  char name[16];
};

extern struct vm vms[VM_MAX];

#endif
