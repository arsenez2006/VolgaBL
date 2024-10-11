.code16
.globl _pre_init

#include <stages/pc-bios.h>
#include <stages/isoboot.h>
#include <version.h>

#define READ_BUFFER ISOBOOT_PRE_SIZE

#define drive -2(%bp)
#define sector_size -4(%bp)
#define dap_lba_high -8(%bp)
#define dap_lba_low -12(%bp)
#define dap_segment -14(%bp)
#define dap_offset -16(%bp)
#define dap_count -18(%bp)
#define dap_size -20(%bp)
#define dap dap_size

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

  /* Check for Extended Fixed Disk Services extension */
  movb $0x41, %ah
  movw $0x55AA, %bx
  int $0x13
  jnc .Lext_check_done
  movw $0x01, %ax
  call _error
.Lext_check_done:

  /* Get drive parameteres */
  movb $0x48, %ah
  movw $READ_BUFFER, %si
  movw $26, (%si)
  int $0x13
  jnc .Lget_drive_done
  mov $0x02, %ax
  call _error
.Lget_drive_done:

  /* Save drive number and sectors count on stack */
  movw %sp, %bp
  xorb %dh, %dh
  pushw %dx
  movw 24(%si), %ax
  pushw %ax

  /* Construct DAP */
  pushl $0 /* LBA high */
  pushl $0 /* LBA low */
  pushw %ds /* Segment */
  pushw $READ_BUFFER /* Offset */
  pushw $0 /* Sector count */
  pushw $16 /* DAP size */

  /* Calculate sector count */
  movw sector_size, %bx
  movw $2048, %ax
  xorw %dx, %dx
  div %bx
  movw %ax, dap_count

  /* Calculate LBA */
  movw $32768, %ax
  xorw %dx, %dx
  div %bx
  movw %ax, dap_lba_low

  movb drive, %dl
  leaw dap, %si
  movb $0x42, %ah
  int $0x13

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
  xorb %bh, %bh
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
