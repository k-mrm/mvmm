#include "uart.h"
#include "gicv3.h"

#define read_sysreg(val, reg) \
  asm volatile("mrs %0, " #reg : "=r"(val))
#define __write_sysreg(reg, val)  \
  asm volatile("msr " #reg ", %0" : : "r"(val))
#define write_sysreg(reg, val)  \
  do { unsigned long x = (unsigned long)(val); __write_sysreg(reg, x); } while(0)

__attribute__((aligned(16))) char _stack[4096];

void el1trap() {
  uart_puts("traaaap");
}

void vectable();

int main(void) {
  uart_init();
  uart_puts("hello, guest world!\n");
  gicv3_init();
  gicv3_init_percpu();
  write_sysreg(vbar_el1, (unsigned long)vectable);
  
  asm volatile("msr daifclr, #0xf" ::: "memory");

  uart_puts("sayonara\n");

  return 0x199999;
}
