#include "types.h"
#include "uart.h"
#include "aarch64.h"
#include "vcpu.h"

#define va_list __builtin_va_list
#define va_start(v, l)  __builtin_va_start(v, l)
#define va_arg(v, l)  __builtin_va_arg(v, l)
#define va_end(v) __builtin_va_end(v)
#define va_copy(d, s) __builtin_va_copy(d, s)

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

static int vprintf(const char *fmt, va_list ap) {
  char *s;
  void *p;

  for(int i = 0; fmt[i]; i++) {
    char c = fmt[i];
    if(c == '%') {
      c = fmt[++i];

      switch(c) {
        case 'd':
          printiu32(va_arg(ap, i32), 10, true);
          break;
        case 'u':
          printiu32(va_arg(ap, u32), 10, false);
          break;
        case 'x':
          printiu64(va_arg(ap, u64), 16, false);
          break;
        case 'p':
          p = va_arg(ap, void *);
          uart_putc('0');
          uart_putc('x');
          printiu64((u64)p, 16, false);
          break;
        case 'c':
          uart_putc(va_arg(ap, int));
          break;
        case 's':
          s = va_arg(ap, char *);
          if(!s)
            s = "(null)";

          uart_puts(s);
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
  va_list ap;
  va_start(ap, fmt);

  vprintf(fmt, ap);

  va_end(ap);

  return 0;
}

void panic(const char *fmt, ...) {
  intr_disable();

  va_list ap;
  va_start(ap, fmt);

  printf("!!!!!!vmm panic: ");
  vprintf(fmt, ap);
  printf("\n");

  vcpu_dump(cur_vcpu());

  va_end(ap);

  for(;;)
    asm volatile("wfi");
}
