.section ".text.boot"

.global _start
_start:
  adrp x0, _stack
  mov x1, #4096
  mrs x2, mpidr_el1
  and x2, x2, #0xf
  add x2, x2, 1
  mul x1, x1, x2
  add x0, x0, x1
  mov sp, x0
  bl vmm_init

 /* spin */
  b .

.section ".text"

.global psci_call
psci_call:
  smc #0
  ret
