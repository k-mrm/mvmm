#include "uart.h"
#include "hello.h"
#include "gicv3.h"

#define intr_enable() \
  asm volatile("msr daifclr, #0x2" ::: "memory");

__attribute__((aligned(16))) char _stack[4096];

int devintr(int iar) {
  int w;
  int irq = iar & 0x3ff;
  uart_puts("devintr\n");

  switch(irq) {
    case UART_IRQ:
      uartintr();
      w = 1;
      break;
    case TIMER_IRQ:
      timerintr();
      w = 1;
      break;
    default:
      uart_puts("????????\n");
      w = 0;
      break;
  }

  if(w)
    gic_eoi(iar);

  return w;
}

void el1trap() {
  uart_puts("el1trap!\n");

  int iar = gic_iar();
  if(!devintr(iar)) {
    uart_puts("sync!");
  }
}

void vectable();

int main(void) {
  uart_init();
  uart_puts("hello, guest world!\n");
  gicv3_init();
  gicv3_init_percpu();
  timerinit();
  write_sysreg(vbar_el1, (unsigned long)vectable);

  intr_enable();

  uart_puts("sayonara\n");

  for(;;)
    intr_enable();

  return 0x199999;
}
