.code16
.globl _pre_init

#include <stages/pc-bios.h>
#include <stages/isoboot.h>
#include <version.h>

.section .text.pre
_pre_init:
  /* Relocate to a well-known address */
  cld
  cli

  movw $PCBIOS_LOAD_SEGMENT, %ax
  movw %ax, %ds
  xorw %si, %si

  movw $ISOBOOT_SEGMENT, %ax
  movw %ax, %es
  xorw %di, %di

  movw $ISOBOOT_PRE_SIZE >> 1, %cx

  rep movsw

  ljmp $ISOBOOT_SEGMENT, $.Lrelocated
.Lrelocated:
  /* Initialize segments */
  movw %ax, %ds
  movw %ax, %ss
  movw $ISOBOOT_STACK, %sp
  sti

  /* Print greet message */
  movw $greet, %si
  xorb %bh, %bh
  movb $0x0E, %ah
.Lgreet_loop:
  lodsb
  orb %al, %al
  jz .Lhalt
  int $0x10
  jmp .Lgreet_loop
.Lhalt:
  hlt
  jmp .Lhalt

.section .rodata.pre
greet:
.ascii "Loading VolgaBL "
.ascii VERSION
.asciz "...\r\n"
