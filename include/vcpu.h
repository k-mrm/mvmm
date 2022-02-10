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
  enum vcpu_state state;
  struct vm *vm;
  int cpuid;
  struct {
    u64 x[31];
    u64 sp;
  } __attribute__((packed)) reg;
  struct {
    u64 spsr_el1;
    u64 elr_el1;
    u64 mpidr_el1;
    u64 midr_el1;
    u64 sp_el0;
    u64 sp_el1;
    u64 ttbr0_el1;
    u64 ttbr1_el1;
    u64 vbar_el1;
  } __attribute__((packed)) sys;
};

extern struct vcpu vcpus[VCPU_MAX];

struct vcpu *new_vcpu(struct vm *vm, int vcpuid, u64 entry);

#endif
