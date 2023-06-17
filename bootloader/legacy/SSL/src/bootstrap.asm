cpu 386
bits 16

global __bootstrap
global __drive_number

extern _ssl_entry

; -------------------------------------------------------------------------------------------------
; BSS
; -------------------------------------------------------------------------------------------------
section .bss
; Allocate stack
stack:
align 16
.bot:
    resb 2 * 1024
.top:

; Saved drive number
__drive_number resb 1

; -------------------------------------------------------------------------------------------------
; TEXT
; -------------------------------------------------------------------------------------------------
section .text
__bootstrap:
    ; Initialize segments and stack
    cli
    cld
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, stack.top
    mov byte [__drive_number], dl ; Save drive number
    sti

    jmp near _ssl_entry

; -------------------------------------------------------------------------------------------------
; RODATA
; -------------------------------------------------------------------------------------------------
section .rodata
msg db "Bootstraped.",0
