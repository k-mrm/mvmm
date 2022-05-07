#include "types.h"
#include "vpsci.h"
#include "log.h"

u64 vpsci_emulate(struct vpsci *vpsci) {
  switch(vpsci->funcid) {
    case PSCI_SYSTEM_OFF:
      /* TODO: shutdown vm */
      break;
    case PSCI_SYSTEM_RESET:
      /* TODO: reboot vm */
      break;
    case PSCI_SYSTEM_CPUON: {
      u64 target_cpu = vpsci->x1;
      u64 ep_addr = vpsci->x2;
      u64 contextid = vpsci->x3;
      vmm_log("cpu%d on: entrypoint %p\n", target_cpu, ep_addr);
      break;
    }
    default:
      vmm_warn("?");
      return -1;
  }

  return 0;
}
