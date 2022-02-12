#include "uart.h"

__attribute__((aligned(16))) char _stack[4096];

int main(void) {
  uart_init();
  uart_puts("hello, guest world!\n");

  return 0x199999;
}
