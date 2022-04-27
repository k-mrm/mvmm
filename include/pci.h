#ifndef MVMM_PCI_H
#define MVMM_PCI_H

#include "aarch64.h"
#include "types.h"

struct pci_config {
  u16 vendor_id;
  u16 device_id;
  u16 command;
  u16 status;
  u8 revision_id;
  u8 prog_if;
  u8 subclass;
  u8 class_code;
  u8 cache_line_sz;
  u8 latency_timer;
  u8 header_type;
  u8 bist;
  u32 bar[6];
  u32 cardbus_cis_ptr;
  u16 subsystem_vendor_id;
  u16 subsystem_id;
  u32 exrom_base;
  u8 cap_ptr;
  u8 res0[3];
  u32 res1;
  u8 intr_line;
  u8 intr_pin;
  u8 min_grant;
  u8 max_latency;
};

struct virtio_pcie_cap {
  u8 cap_vndr;
  u8 cap_next;
  u8 cap_len;
  u8 cfg_type;
  u8 bar;
  u8 padding[3];
  u32 offset;
  u32 length;
};

struct virtio_pcie_cfg_cap {
  struct virtio_pcie_cap cap;
  u8 pci_config_data[4];
};

struct pcie_cap {
  u8 cap_id;
  u8 next_cap;
  u16 cap_reg;
  u32 dev_cap;
};

/* Common configuration */ 
#define VIRTIO_PCI_CAP_COMMON_CFG        1
/* Notifications */ 
#define VIRTIO_PCI_CAP_NOTIFY_CFG        2
/* ISR Status */ 
#define VIRTIO_PCI_CAP_ISR_CFG           3
/* Device specific configuration */ 
#define VIRTIO_PCI_CAP_DEVICE_CFG        4
/* PCI configuration access */ 
#define VIRTIO_PCI_CAP_PCI_CFG           5

#endif
