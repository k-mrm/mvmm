#include "types.h"
#include "vpsci.h"
#include "log.h"

static u64 vpsci_cpu_on(struct vcpu *vcpu, struct vpsci *vpsci) {
  u64 target_cpu = vpsci->x1;
  u64 ep_addr = vpsci->x2;
  u64 contextid = vpsci->x3;
  vmm_log("cpu%d on: entrypoint %p\n", target_cpu, ep_addr);

  if(target_cpu >= vcpu->vm->nvcpu) {
    vmm_warn("cpu%d wakeup failed\n", target_cpu);
    return -1;
  }

  struct vcpu *target = vcpu->vm->vcpus[target_cpu];
  target->reg.elr = ep_addr;

  return 0;
}

u64 vpsci_emulate(struct vcpu *vcpu, struct vpsci *vpsci) {
  switch(vpsci->funcid) {
    case PSCI_SYSTEM_OFF:
      /* TODO: shutdown vm */
      break;
    case PSCI_SYSTEM_RESET:
      /* TODO: reboot vm */
      break;
    case PSCI_SYSTEM_CPUON:
      return vpsci_cpu_on(vcpu, vpsci);
    default:
      vmm_warn("?");
      return -1;
  }

  return 0;
}
