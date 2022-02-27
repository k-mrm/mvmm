#include "types.h"
#include "aarch64.h"
#include "printf.h"
#include "vcpu.h"
#include "log.h"

void hyp_sync_handler() {
  panic("sync el2");
}

void hyp_irq_handler() {
  panic("irq el2");
}

void vm_irq_handler() {
  vmm_log("vm_irq_handler hello\n");

  struct vcpu *vcpu;
  read_sysreg(vcpu, tpidr_el2);

  u32 pirq = gic_read_irq();
  u32 virq = pirq;

  vgic_lr_pending(vcpu->vm->vgic, pirq, virq, 1);
}

void vm_sync_handler() {
  struct vcpu *vcpu;
  read_sysreg(vcpu, tpidr_el2);

  vmm_log("el0/1 sync!\n");

  u64 esr, elr, far, daif;
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
      vmm_log("hvc ");
      print64(iss);
      printf("\n");
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
