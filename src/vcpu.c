#include "aarch64.h"
#include "vcpu.h"
#include "printf.h"
#include "lib.h"
#include "pcpu.h"

struct vcpu vcpus[VCPU_MAX];

static void save_sysreg(struct vcpu *vcpu);
static void restore_sysreg(struct vcpu *vcpu);

static struct vcpu *allocvcpu() {
  for(struct vcpu *vcpu = vcpus; vcpu < &vcpus[VCPU_MAX]; vcpu++) {
    if(vcpu->state == UNUSED) {
      vcpu->state = CREATED;
      return vcpu;
    }
  }

  return NULL;
}

struct vcpu *new_vcpu(struct vm *vm, int vcpuid, u64 entry) {
  struct vcpu *vcpu = allocvcpu();
  if(!vcpu)
    panic("vcpu kokatsu");

  vcpu->name = "cortex-a72";
  vcpu->vm = vm;
  vcpu->cpuid = vcpuid;

  vcpu->sys.spsr_el2 = 0x3c5;   /* EL1 */
  vcpu->sys.elr_el2 = entry;
  vcpu->sys.mpidr_el1 = vcpuid; /* TODO: affinity */
  vcpu->sys.midr_el1 = 0x410fd081;  /* cortex-a72 */

  return vcpu;
}

void free_vcpu(struct vcpu *vcpu) {
  vcpu->state = UNUSED;

  memset(vcpu, 0, sizeof(*vcpu));
}

void schedule() {
  struct pcpu *pcpu = cur_pcpu();

  for(;;) {
    for(struct vcpu *vcpu = vcpus; vcpu < &vcpus[VCPU_MAX]; vcpu++) {
      if(vcpu->state == READY) {
        struct vm *vm = vcpu->vm;

        pcpu->vcpu = vcpu;

        vcpu->state = RUNNING;
        write_sysreg(vttbr_el2, vm->vttbr);
        restore_sysreg(vcpu);

        asm volatile("eret");

        pcpu->vcpu = NULL;
      }
    }
  }
}

static void save_sysreg(struct vcpu *vcpu) {
  read_sysreg(vcpu->sys.spsr_el2, spsr_el2);
  read_sysreg(vcpu->sys.elr_el2, elr_el2);
  read_sysreg(vcpu->sys.spsr_el1, spsr_el1);
  read_sysreg(vcpu->sys.elr_el1, elr_el1);
  read_sysreg(vcpu->sys.mpidr_el1, mpidr_el1);
  read_sysreg(vcpu->sys.midr_el1, midr_el1);
  read_sysreg(vcpu->sys.sp_el0, sp_el0);
  read_sysreg(vcpu->sys.sp_el1, sp_el1);
  read_sysreg(vcpu->sys.ttbr0_el1, ttbr0_el1);
  read_sysreg(vcpu->sys.ttbr1_el1, ttbr1_el1);
  read_sysreg(vcpu->sys.vbar_el1, vbar_el1);
}

static void restore_sysreg(struct vcpu *vcpu) {
  write_sysreg(spsr_el2, vcpu->sys.spsr_el2);
  write_sysreg(elr_el2, vcpu->sys.elr_el2);
  write_sysreg(spsr_el1, vcpu->sys.spsr_el1);
  write_sysreg(elr_el1, vcpu->sys.elr_el1);
  write_sysreg(vmpidr_el2, vcpu->sys.mpidr_el1);
  write_sysreg(vpidr_el2, vcpu->sys.midr_el1);
  write_sysreg(sp_el0, vcpu->sys.sp_el0);
  write_sysreg(sp_el1, vcpu->sys.sp_el1);
  write_sysreg(ttbr0_el1, vcpu->sys.ttbr0_el1);
  write_sysreg(ttbr1_el1, vcpu->sys.ttbr1_el1);
  write_sysreg(vbar_el1, vcpu->sys.vbar_el1);
}
