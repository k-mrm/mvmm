#include "uart.h"
#include "gicv3.h"
#include "hello.h"

__attribute__((aligned(16))) char _stack[4096];

void el1trap() {
  timerintr();
}

void vectable();

int main(void) {
  uart_init();
  uart_puts("hello, guest world!\n");
  gicv3_init();
  gicv3_init_percpu();
  timerinit();
  write_sysreg(vbar_el1, (unsigned long)vectable);

  asm volatile("msr daifclr, #0x2" ::: "memory");

  uart_puts("sayonara\n");

  return 0x199999;
}
