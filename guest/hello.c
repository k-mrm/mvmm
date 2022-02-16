#include "uart.h"
#include "gicv3.h"

__attribute__((aligned(16))) char _stack[4096];

int main(void) {
  uart_init();
  uart_puts("hello, guest world!\n");
  gicv3_init();
  gicv3_init_percpu();

  return 0x199999;
}
