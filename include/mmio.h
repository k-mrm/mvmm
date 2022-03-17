#ifndef MVMM_MMIO_H
#define MVMM_MMIO_H

#include "vcpu.h"

enum mmio_type {
  PASSTHROUGH,
  EMULATE,
};

struct mmio_info {
  u64 base;
  u64 size;
  int (*read)(struct vcpu *, int, u64, int);
  int (*write)(struct vcpu *, int, u64, int);
};

int mmio_emulate(struct vcpu *vcpu, int reg, u64 ipa, int accsize, bool wr);

extern struct mmio_info virtmap[];

#endif
