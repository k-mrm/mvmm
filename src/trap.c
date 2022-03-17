#include "types.h"
#include "aarch64.h"
#include "param.h"
#include "printf.h"
#include "vcpu.h"
#include "log.h"
#include "mm.h"
#include "mmio.h"

void hyp_sync_handler() {
  panic("sync el2");
}

void hyp_irq_handler() {
  panic("irq el2");
}

void vm_irq_handler() {
  vmm_log("vm_irq_handler\n");

  struct vcpu *vcpu;
  read_sysreg(vcpu, tpidr_el2);
  struct vgic *vgic = vcpu->vm->vgic;

  vgic_irq_enter(vgic);

  u32 iar = gic_read_iar();
  u32 pirq = iar & 0x3ff;
  u32 virq = pirq;

  // drop primary
  gic_eoi(iar, 1);

  vgic_forward_virq(vcpu->vm->vgic, pirq, virq, 1);

  // gic_deactive_int(pirq);
}

static u64 faulting_ipa(u64 vaddr) {
  u64 hpfar;
  read_sysreg(hpfar, hpfar_el2);

  u64 ipa_page = (hpfar & HPFAR_FIPA_MASK) << 8;

  return ipa_page | (vaddr & (PAGESIZE-1));
}

int vm_dabort_handler(struct vcpu *vcpu, u64 iss, u64 far) {
  u64 ipa = faulting_ipa(far);

  bool wnr = (iss >> 6) & 0x1;
  int sas = (iss >> 22) & 0x3;
  int r = (iss >> 16) & 0x1f;

  int accsize;
  switch(sas) {
    case 0: accsize = 8; break;
    case 1: accsize = 16; break;
    case 2: accsize = 32; break;
    case 3: accsize = 64; break;
    default: panic("?");
  }

  if(mmio_emulate(vcpu, r, ipa, accsize, wnr) < 0)
    return -1;

  return 0;
}

void vm_sync_handler() {
  struct vcpu *vcpu;
  read_sysreg(vcpu, tpidr_el2);

  vmm_log("el0/1 sync!\n");

  u64 esr, elr, far;
  read_sysreg(esr, esr_el2);
  read_sysreg(elr, elr_el2);
  read_sysreg(far, far_el2);
  u64 ec = (esr >> 26) & 0x3f;
  u64 iss = esr & 0x1ffffff;

  switch(ec) {
    case 0x1:     /* WF* */
      vmm_log("wf* trapped\n");
      vcpu->reg.elr += 4;
      break;
    case 0x16:    /* hvc */
      vmm_log("hvc %x\n", iss);
      break;
    case 0x24:    /* data abort */
      if(vm_dabort_handler(vcpu, iss, far) < 0)
        panic("dabort %x\n", iss);

      break;
    default:
      vmm_log("%x %x %x %x\n", ec, iss, elr, far);
      panic("unknown sync");
  }
}
