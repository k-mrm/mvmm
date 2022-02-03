.section ".text.boot"

.global _start
_start:
  mrs x1, mpidr_el1
  and x1, x1, #3
  cbz x1, hyp_start

hyp_start:
  mrs x0, currentel
  b .
