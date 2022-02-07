#include "uart.h"
#include "aarch64.h"
#include "pmalloc.h"
#include "pcpu.h"
#include "printf.h"

__attribute__((aligned(16))) char _stack[4096*NCPU];

int vmm_init() {
  uart_init();
  printf("mvmm booting...\n");
  pmalloc_init();

  printf("hello\n");

  return 0;
}
