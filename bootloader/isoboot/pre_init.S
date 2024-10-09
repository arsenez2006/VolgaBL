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
  call _print_msg

  movw $0xDEAD, %ax
  call _error
.Lhalt:
  hlt
  jmp .Lhalt

/* Prints message from SI */
_print_msg:
  pushaw
  xorb %bh, %bh
  movb $0x0E, %ah
.Lprint_loop:
  lodsb
  orb %al, %al
  jz .Lprint_done
  int $0x10
  jmp .Lprint_loop
.Lprint_done:
  popaw
  ret

/* Prints error message with error code from AX and halt processor */
_error:
  movw %ax, %dx
  movw $error_msg, %si
  call _print_msg
  movw $4, %cx
  movb $0x0E, %ah
.Lnum_loop:
  movb %dh, %bl
  shrb $4, %bl
  movb hex_alphabet(%bx), %al
  int $0x10
  shlw $4, %dx
  loop .Lnum_loop
.Lehalt:
  hlt
  jmp .Lehalt

.section .rodata.pre
greet:
.ascii "Loading VolgaBL "
.ascii VERSION
.asciz "...\r\n"

error_msg:
.asciz "ISOBOOT PRE error: 0x"

hex_alphabet:
.ascii "0123456789ABCDEF"
