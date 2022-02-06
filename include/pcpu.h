#ifndef MVMM_PCPU_H
#define MVMM_PCPU_H

#include "types.h"

#define NCPU  1

struct pcpu {
  int cpuid;
};

extern struct pcpu pcpus[NCPU];

#endif
