#ifndef MVMM_PCPU_H
#define MVMM_PCPU_H

#include "types.h"
#include "vcpu.h"

#define NCPU  1

struct pcpu {
  int cpuid;
  struct vcpu *vcpu;  /* current vcpu */
};

extern struct pcpu pcpus[NCPU];

#endif
