#ifndef MVMM_VPSCI_H
#define MVMM_VPSCI_H

#include "types.h"

#define PSCI_SYSTEM_OFF   0x84000008
#define PSCI_SYSTEM_RESET   0x84000009
#define PSCI_SYSTEM_CPUON   0xc4000003

struct vpsci {
  u32 funcid;
  u64 x1;
  u64 x2;
  u64 x3;
};

u64 vpsci_emulate(struct vpsci *vpsci);

#endif
