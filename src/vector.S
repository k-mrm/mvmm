.section ".text"

.global vectable
.balign 0x800
vectable:
/* current EL with sp0 */
  b .
.balign 0x80
  b .
.balign 0x80
  b .
.balign 0x80
  b .

/* current EL with spx */
.balign 0x80
  b el2_sync
.balign 0x80
  b el2_irq
.balign 0x80
  b .
.balign 0x80
  b .

/* lower EL using aarch64 */
.balign 0x80
  b el1_sync
.balign 0x80
  b el1_irq
.balign 0x80
  b .
.balign 0x80
  b .

/* lower EL using aarch32 */
.balign 0x80
  b .
.balign 0x80
  b .
.balign 0x80
  b .
.balign 0x80
  b .

el2_sync:
  b .
el2_irq:
  b .

el1_sync:
  bl el1_sync_handler
el1_irq:
  b .
