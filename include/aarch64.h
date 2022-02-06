#ifndef MVMM_AARCH64_H
#define MVMM_AARCH64_H

#define read_sysreg(val, reg) \
  asm volatile("mrs %0, " #reg : "=r"(val))
#define write_sysreg(reg, val)  \
  asm volatile("msr " #reg ", %0" : : "r"(val))

#endif
