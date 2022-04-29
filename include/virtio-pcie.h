#ifndef MVMM_VIRTIO_PCIE_H
#define MVMM_VIRTIO_PCIE_H

#include "aarch64.h"
#include "types.h"

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
