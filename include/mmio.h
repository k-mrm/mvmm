#ifndef MVMM_MMIO_H
#define MVMM_MMIO_H

#include "types.h"
#include "vcpu.h"

enum mmio_accsize {
  ACC_BYTE = 1<<0,
  ACC_HALFWORD = 1<<1,
  ACC_WORD = 1<<2,
  ACC_DOUBLEWORD = 1<<3,
};

enum mmio_type {
  PASSTHROUGH,
  EMULATE,
};

struct mmio_info {
  u64 base;
  u64 size;
  int (*read)(struct vcpu *, u64, u64 *, enum mmio_accsize);
  int (*write)(struct vcpu *, u64, u64, enum mmio_accsize);
};

int mmio_emulate(struct vcpu *vcpu, u64 ipa, u64 *reg, enum mmio_accsize accsize, bool wr);

extern struct mmio_info virtmap[];

#endif
