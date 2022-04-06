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

extern struct guest hello;
void vectable();

__attribute__((aligned(16))) char _stack[4096*NCPU];

int vmm_init() {
  uart_init();
  vmm_log("mvmm booting...\n");
  pmalloc_init();
  pcpu_init();
  gic_init();
  gic_init_cpu(0);
  vtimer_init();

  u64 vtcr = VTCR_T0SZ(25) | VTCR_SH0(0) | VTCR_SL0(1) | VTCR_TG0(0);
  write_sysreg(vtcr_el2, vtcr);

  u64 mair = (AI_DEVICE_nGnRnE << (8 * AI_DEVICE_nGnRnE_IDX)) | (AI_NORMAL_NC << (8 * AI_NORMAL_NC_IDX));
  write_sysreg(mair_el2, mair);

  write_sysreg(vbar_el2, (u64)vectable);

  u64 hcr = HCR_VM | HCR_SWIO | HCR_FMO | HCR_IMO |
            HCR_TWI | HCR_TWE | HCR_RW;
  write_sysreg(hcr_el2, hcr);

  isb();

  intr_enable();

  new_vm("hello", 1, hello.start, hello.size, 0x40000000, 256*1024);

  enter_vcpu();
}
