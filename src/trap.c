#include "types.h"
#include "aarch64.h"
#include "printf.h"

void el1_sync_handler() {
  printf("el1sync!\n");
  u64 esr, elr, far;
  read_sysreg(esr, esr_el2);
  read_sysreg(elr, elr_el2);
  read_sysreg(far, far_el2);
  print64((esr >> 26) & 0x3f);
  printf("\n");
  print64(elr);
  printf("\n");
  print64(far);
}
