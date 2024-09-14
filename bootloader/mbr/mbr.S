.code16
.globl _entry

.section .text
_entry:
  cli
  cld
  /* Initialize segments */
  xorw %ax, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movw $0x7C00, %sp

  /* Relocate to lower address */
  movw $0x0100, %cx
  movw $0x7C00, %si
  movw $0x1000, %di
  rep movsw

  /* Jump to relocated code */
  ljmp $0x0, $_relocated/* FIXME: Why the fuck ld segfaults when I put $_relocated here??? */
_relocated:
  sti
.halt:
  hlt
  jmp .halt
