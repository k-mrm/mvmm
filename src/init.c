#include "uart.h"
#include "aarch64.h"
#include "pmalloc.h"
#include "pcpu.h"

__attribute__((aligned(16))) char _stack[4096*NCPU];

int vmm_init() {
  uart_init();
  uart_puts("mvmm sttttttttttart\n");
  pmalloc_init();

  return 0;
}
