#ifndef MVMM_VM_H
#define MVMM_VM_H

#include "types.h"

#define VM_MAX  2

struct vm {
  u64 *vttbr;
  char name[16];
  u64 entry;
  int used;
};

extern struct vm vms[VM_MAX];

void new_vm(char *name, int ncpu, u64 img_start, u64 img_size, u64 entry, u64 allocated);

#endif
