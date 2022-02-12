#ifndef MVMM_AARCH64_H
#define MVMM_AARCH64_H

#define read_sysreg(val, reg) \
  asm volatile("mrs %0, " #reg : "=r"(val))
#define write_sysreg(reg, val)  \
  asm volatile("msr " #reg ", %0" : : "r"(val))

#define intr_enable()   asm volatile("msr daifclr, #2" ::: "memory")
#define intr_disable()  asm volatile("msr daifset, #2" ::: "memory")

#define HCR_VM    (1<<0)
#define HCR_TWI   (1<<13)
#define HCR_TWE   (1<<14)
#define HCR_RW    (1<<31)

static inline int cpuid() {
  int mpidr;
  read_sysreg(mpidr, mpidr_el1);
  return mpidr & 0xf;
}

#endif
