#include "memmap.h"
#include "pci.h"
#include "log.h"

struct pci_config_space {
  u8 raw[256][32][8][4096];
};

static void pcie_scan_dev() {
  struct pci_config *cfg;
  struct pci_config_space *space = (struct pci_config_space *)PCIE_ECAM_BASE;

  for(int bc = 0; bc < 256; bc++)
    for(int dc = 0; dc < 32; dc++)
      for(int fc = 0; fc < 8; fc++) {
        cfg = (struct pci_config *)&space->raw[bc][dc][fc];
        if(cfg->vendor_id == 0xffff)
          continue;

        vmm_log("pcie find: %d\n", cfg->vendor_id);

        /* VIRTIO vendor id */
        if(cfg->vendor_id == 0x1af4) {
          vmm_log("\tvirtio %d sub:%d\n", cfg->device_id, cfg->subsystem_id);
          vmm_log("\theader %d, cap->ptr %d\n", cfg->header_type, cfg->cap_ptr);
          vmm_log("\tcommand %p status %p\n", cfg->command, cfg->status);

          cfg->command |= (1<<0) | (1<<1) | (1<<2);
          vmm_log("\tcommand %x\n", cfg->command);
          virtio_pci_dev_init(cfg);
        }
      }
}

void pcie_init() {
  pcie_scan_dev();
}
