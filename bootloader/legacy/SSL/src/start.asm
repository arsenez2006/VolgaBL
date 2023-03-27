cpu 386
bits 16

global __start
global _drive_number

extern _ssl_entry
extern _enter_unreal_mode

section .bss
; Allocate stack
stack:
align 16
.top:
    resb 1024 * 4
.bottom:
; Drive number
_drive_number:
    resb 1

section .text
__start:
    ; Initialize segments
    cli
    mov ax, 0x0800
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, stack.bottom

    ; Save drive number
    mov byte [_drive_number], dl

    sti

    ; Call C code
    call _ssl_entry

.halt:
    hlt
    jmp short .halt
