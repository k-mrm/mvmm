#include "uart.h"
#include "aarch64.h"
#include "pmalloc.h"
#include "guest.h"
#include "vm.h"
#include "pcpu.h"
#include "mm.h"
#include "log.h"
#include "vgic.h"
#include "vtimer.h"
#include "pci.h"
#include "log.h"

extern struct guest hello;
void vectable();

__attribute__((aligned(16))) char _stack[4096*NCPU];

volatile static int cpu0_ready;

static void hcr_setup() {
  u64 hcr = HCR_VM | HCR_SWIO | HCR_FMO | HCR_IMO |
            HCR_TWI | HCR_TWE | HCR_RW;

  write_sysreg(hcr_el2, hcr);

  isb();
}

int vmm_init() {
  if(cpuid() == 0) {
    uart_init();
    vmm_log("mvmm booting...\n");
    pmalloc_init();
    pcpu_init();
    write_sysreg(vbar_el2, (u64)vectable);
    gic_init();
    gic_init_cpu(0);
    vtimer_init();
    s2mmu_init();
    pci_init();
    hcr_setup();

    new_vm("hello", 2, hello.start, hello.size, 0x40000000, 128*1024*1024 /* 128 MiB */);
    cpu0_ready = 1;
  } else {
    while(!cpu0_ready)
      ;
    write_sysreg(vbar_el2, (u64)vectable);
    gic_init_cpu(0);
    s2mmu_init();
    hcr_setup();
  }

  schedule();

  for(;;)
    ;
}
