cpu 386
bits 16
global __start

section .bss
; Allocate stack
stack:
align 16
.top:
    resb 1024 * 4
.bottom:

section .text
extern _ssl_entry

__start:
    ; Initialize segments
    cli
    mov ax, 0x0800
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, stack.bottom
    sti

    ; Call C code
    call _ssl_entry

.halt:
    hlt
    jmp short .halt
