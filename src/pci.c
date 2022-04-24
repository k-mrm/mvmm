#include "memmap.h"
#include "pci.h"
#include "log.h"

#define R(reg)  (volatile u32 *)(PCIE_ECAM_BASE+reg)

void pcie_init() {
  u32 c = *R(0);
  vmm_log("pcie %c\n", c);
}
