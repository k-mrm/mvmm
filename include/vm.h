#ifndef MVV_VM_H
#define MVV_VM_H

#include "types.h"

#define VM_MAX  2

struct vm {
  u64 *vttbr;
};

extern struct vm vms[VM_MAX];

#endif
