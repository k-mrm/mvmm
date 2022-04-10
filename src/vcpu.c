#include "aarch64.h"
#include "vcpu.h"
#include "printf.h"
#include "lib.h"
#include "pcpu.h"
#include "log.h"

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

  vcpu->vgic = new_vgic_cpu(vcpuid);

  vcpu->reg.spsr = 0x3c5;   /* EL1 */
  vcpu->reg.elr = entry;

  vcpu->sys.mpidr_el1 = vcpuid; /* TODO: affinity */
  vcpu->sys.midr_el1 = 0x410fd081;  /* cortex-a72 */
  vcpu->sys.sctlr_el1 = 0xc50838;
  vcpu->sys.cntfrq_el0 = 62500000;

  gic_init_state(&vcpu->gic);

  return vcpu;
}

void free_vcpu(struct vcpu *vcpu) {
  vcpu->state = UNUSED;

  memset(vcpu, 0, sizeof(*vcpu));
}

void trapret(void);

static void switch_vcpu(struct pcpu *pcpu, struct vcpu *vcpu) {
  write_sysreg(vttbr_el2, vcpu->vm->vttbr);

  restore_sysreg(vcpu);

  gic_restore_state(&vcpu->gic);

  /* enter vm */
  trapret();

  /* now unreachable */
}

void vcpu_ready(struct vcpu *vcpu) {
  struct pcpu *pcpu = cur_pcpu();

  vcpu->state = READY;

  struct vcpu **v = &pcpu->ready;
  while(*v)
    v = &(*v)->next;

  *v = vcpu;
}

struct vcpu *schedule() {
  struct pcpu *pcpu = cur_pcpu();

  if(!pcpu->ready)
    return NULL;

  struct vcpu *vcpu = pcpu->ready;
  pcpu->ready = vcpu->next;
  vcpu->next = NULL;

  return vcpu;
}

void enter_vcpu() {
  struct vcpu *vcpu = schedule();

  cur_pcpu()->vcpu = vcpu;
  write_sysreg(tpidr_el2, vcpu);

  vcpu->state = RUNNING;

  write_sysreg(vttbr_el2, vcpu->vm->vttbr);
  restore_sysreg(vcpu);
  gic_restore_state(&vcpu->gic);

  /* enter vm */
  trapret();
}

/* vcpu gives up pcpu */
void yield() {
  struct pcpu *p = cur_pcpu();
  struct vcpu *v = p->vcpu;

  p->vcpu = NULL;
  write_sysreg(tpidr_el2, 0);
  write_sysreg(vttbr_el2, 0);

  save_sysreg(v);
  gic_save_state(&v->gic);

  vcpu_ready(v);

  enter_vcpu();
}

/*
void schedule() {
  struct pcpu *pcpu = cur_pcpu();

  for(;;) {
    for(struct vcpu *vcpu = vcpus; vcpu < &vcpus[VCPU_MAX]; vcpu++) {
      if(vcpu->state == READY) {
        pcpu->vcpu = vcpu;
        write_sysreg(tpidr_el2, vcpu);
        vcpu->state = RUNNING;

        vmm_log("cpu%d: entering vm `%s`\n", pcpu->cpuid, vcpu->vm->name);

        switch_vcpu(pcpu, vcpu);

        pcpu->vcpu = NULL;
        write_sysreg(tpidr_el2, 0);
      }
    }
  }
}
*/

static void save_sysreg(struct vcpu *vcpu) {
  read_sysreg(vcpu->sys.spsr_el1, spsr_el1);
  read_sysreg(vcpu->sys.elr_el1, elr_el1);
  // read_sysreg(vcpu->sys.mpidr_el1, mpidr_el1);
  // read_sysreg(vcpu->sys.midr_el1, midr_el1);
  read_sysreg(vcpu->sys.sp_el0, sp_el0);
  read_sysreg(vcpu->sys.sp_el1, sp_el1);
  read_sysreg(vcpu->sys.ttbr0_el1, ttbr0_el1);
  read_sysreg(vcpu->sys.ttbr1_el1, ttbr1_el1);
  read_sysreg(vcpu->sys.tcr_el1, tcr_el1);
  read_sysreg(vcpu->sys.vbar_el1, vbar_el1);
  read_sysreg(vcpu->sys.sctlr_el1, sctlr_el1);
  read_sysreg(vcpu->sys.cntv_ctl_el0, cntv_ctl_el0);
  read_sysreg(vcpu->sys.cntv_tval_el0, cntv_tval_el0);
}

static void restore_sysreg(struct vcpu *vcpu) {
  write_sysreg(spsr_el1, vcpu->sys.spsr_el1);
  write_sysreg(elr_el1, vcpu->sys.elr_el1);
  write_sysreg(vmpidr_el2, vcpu->sys.mpidr_el1);
  write_sysreg(vpidr_el2, vcpu->sys.midr_el1);
  write_sysreg(sp_el0, vcpu->sys.sp_el0);
  write_sysreg(sp_el1, vcpu->sys.sp_el1);
  write_sysreg(ttbr0_el1, vcpu->sys.ttbr0_el1);
  write_sysreg(ttbr1_el1, vcpu->sys.ttbr1_el1);
  write_sysreg(tcr_el1, vcpu->sys.tcr_el1);
  write_sysreg(vbar_el1, vcpu->sys.vbar_el1);
  write_sysreg(sctlr_el1, vcpu->sys.sctlr_el1);
  write_sysreg(cntv_ctl_el0, vcpu->sys.cntv_ctl_el0);
  write_sysreg(cntv_tval_el0, vcpu->sys.cntv_tval_el0);
  write_sysreg(cntfrq_el0, vcpu->sys.cntfrq_el0);
}
