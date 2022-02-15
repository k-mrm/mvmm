#include "types.h"
#include "aarch64.h"
#include "printf.h"
#include "vcpu.h"
#include "log.h"

void hyp_sync_handler() {
  panic("sync el2");
}

void vm_sync_handler() {
  struct vcpu *vcpu;
  read_sysreg(vcpu, tpidr_el2);

  vmm_log("el1sync!\n");

  u64 esr, elr, far;
  read_sysreg(esr, esr_el2);
  read_sysreg(elr, elr_el2);
  read_sysreg(far, far_el2);
  u64 ec = (esr >> 26) & 0x3f;
  u64 iss = esr & 0x1ffffff;

  switch(ec) {
    case 0b000001:    /* WF* */
      vmm_log("wf* trapped\n");
      vcpu->reg.elr += 4;
      break;
    default:
      print64(ec);
      printf("\n");
      print64(iss);
      printf("\n");
      print64(elr);
      printf("\n");
      print64(far);
      panic("unknown sync");
  }
}
