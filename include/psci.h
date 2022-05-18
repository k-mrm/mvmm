#ifndef MVMM_PSCI_H
#define MVMM_PSCI_H

#define PSCI_SYSTEM_OFF     0x84000008
#define PSCI_SYSTEM_RESET   0x84000009
#define PSCI_SYSTEM_CPUON   0xc4000003

void psci_call(u32 func, u64 cpuid, u64 entry, u64 ctxid);

#endif
