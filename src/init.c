#include "uart.h"

__attribute__((aligned(16))) char _stack[4096];

int vmm_init() {
  uart_init();

  uart_puts("mvmm sttttttttttart\n");

  return 0;
}
