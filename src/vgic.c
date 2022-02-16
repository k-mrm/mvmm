#include "aarch64.h"

/* gicv3 controller */

#define ich_hcr_el2   sysreg32(4, c12, c11, 0)

#define ICH_HCR_EN  (1<<0)

void vgic_init(void) {
  write_sysreg(ich_hcr_el2, ICH_HCR_EN);
}
