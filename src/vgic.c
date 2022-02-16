#include "aarch64.h"

/* gicv3 controller */

#define ICH_HCR_EN  (1<<0)

void vgic_init(void) {
  u64 hcr = ICH_HCR_EN;
  write_sysreg(ich_hcr_el2, hcr);
}
