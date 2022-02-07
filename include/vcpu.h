#ifndef MVMM_VCPU_H
#define MVMM_VCPU_H

#include "types.h"

#define VCPU_MAX 4

struct vcpu {
  struct vcpu *next;
};

extern struct vcpu vcpus[VCPU_MAX];

#endif
