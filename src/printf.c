#include "types.h"
#include "uart.h"
#include "aarch64.h"

static void printiu32(i32 num, int base, bool sign) {
  char buf[sizeof(num) * 8 + 1] = {0};
  char *end = buf + sizeof(buf);
  char *cur = end - 1;
  u32 unum;
  bool neg = false;

  if(sign && num < 0) {
    unum = (u32)(-(num + 1)) + 1;
    neg = true;
  } else {
    unum = (u32)num;
  }

  do {
    *--cur = "0123456789abcdef"[unum % base];
  } while(unum /= base);

  if(neg)
    *--cur = '-';

  uart_puts(cur);
}

static void printiu64(i64 num, int base, bool sign) {
  char buf[sizeof(num) * 8 + 1] = {0};
  char *end = buf + sizeof(buf);
  char *cur = end - 1;
  u64 unum;
  bool neg = false;

  if(sign && num < 0) {
    unum = (u64)(-(num + 1)) + 1;
    neg = true;
  } else {
    unum = (u64)num;
  }

  do {
    *--cur = "0123456789abcdef"[unum % base];
  } while(unum /= base);

  if(neg)
    *--cur = '-';

  uart_puts(cur);
}

static int vprintf(const char *fmt) {
  for(int i = 0; fmt[i]; i++) {
    char c = fmt[i];
    if(c == '%') {
      c = fmt[++i];

      switch(c) {
        case 'd':
          break;
        case 'u':
          break;
        case 'x':
          break;
        case 'p':
          break;
        case 'c':
          break;
        case 's':
          break;
        case '%':
          uart_putc('%');
          break;
        default:
          uart_putc('%');
          uart_putc(c);
          break;
      }
    } else {
      uart_putc(c);
    }
  }

  return 0;
}

int printf(const char *fmt, ...) {
  return vprintf(fmt);
}

void panic(const char *fmt) {
  printf("!!!vmm panic ");
  vprintf(fmt);
  printf("\n");

  intr_disable();

  for(;;)
    asm volatile("wfi");
}
