#include <version.h>
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
  movw %sp, %si
  movw $_entry, %di
  rep movsw

  /* Jump to relocated code */
  ljmp $0x0000, $.Lrelocated 
.Lrelocated:
  sti

.Lhalt:
  hlt
  jmp .Lhalt

.section .rodata
greet: .ascii "Loading VolgaBL "
version: .ascii VERSION
greet_end: .asciz "... "
