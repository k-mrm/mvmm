#ifndef MVMM_VCPU_H
#define MVMM_VCPU_H

#include "types.h"
#include "vm.h"

#define VCPU_MAX 4

enum vcpu_state {
  UNUSED,
  CREATED,
  READY,
  RUNNING,
  IDLE,
};

struct vcpu {
  struct vcpu *next;
  enum vcpu_state state;
  struct vm *vm;
};

extern struct vcpu vcpus[VCPU_MAX];

struct vcpu *allocvcpu(void);

#endif
