#include "aarch64.h"
#include "vgic.h"
#include "log.h"

/* gicv3 controller */

#define ich_hcr_el2   sysreg32(4, c12, c11, 0)
#define ich_vtr_el2   sysreg32(4, c12, c11, 1)
#define ich_vmcr_el2  sysreg32(4, c12, c11, 7)

#define ICH_HCR_EN  (1<<0)

#define ICH_VMCR_VENG0  (1<<0)
#define ICH_VMCR_VENG1  (1<<1)

void vgic_init(void) {
  vmm_log("vgic init...\n");
  write_sysreg(ich_vmcr_el2, ICH_VMCR_VENG1);
  write_sysreg(ich_hcr_el2, ICH_HCR_EN);
}
