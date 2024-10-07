.code16
.globl _pre_init

#include <stages/pc-bios.h>
#include <stages/isoboot.h>

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
  
  ljmp $ISOBOOT_SEGMENT, $.Lrelocated
.Lrelocated:
  
.Lhalt:
  hlt
  jmp .Lhalt
