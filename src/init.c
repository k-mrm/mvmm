#include "uart.h"
#include "aarch64.h"
#include "pmalloc.h"
#include "printf.h"
#include "guest.h"
#include "vm.h"
#include "pcpu.h"

extern struct guest hello;
void vectable();

__attribute__((aligned(16))) char _stack[4096*NCPU];

int vmm_init() {
  uart_init();
  printf("mvmm booting...\n");
  pmalloc_init();
  printf("hello\n");

  u64 hcr = HCR_VM |
            HCR_TWI |
            HCR_TWE |
            HCR_RW;
  write_sysreg(hcr_el2, hcr);

  write_sysreg(vbar_el2, (u64)vectable);

  new_vm("hello", 1, hello.start, hello.size, 0x40000000, 256*1024);

  schedule();
}
