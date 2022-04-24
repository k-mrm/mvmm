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
  u32 bar0;
  u32 bar1;
  u32 bar2;
  u32 bar3;
  u32 bar4;
  u32 bar5;
  u32 cardbus_cis_ptr;
  u16 subsystem_vendor_id;
  u16 subsystem_id;
  u32 exrom_base;
  u8 capabilities_ptr;
  u8 res0[3];
  u32 res1;
  u8 intr_line;
  u8 intr_pin;
  u8 min_grant;
  u8 max_latency;
};

#endif
