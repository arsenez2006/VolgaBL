cpu 386
bits 16

global __bootstrap
global __drive_number

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

    mov si, msg
.loop:
    lodsb
    or al, al
    jz short .halt
    mov ah, 0x0E
    int 0x10
    jmp short .loop

.halt:
    hlt
    jmp short .halt

; -------------------------------------------------------------------------------------------------
; RODATA
; -------------------------------------------------------------------------------------------------
section .rodata
msg db "Bootstraped.",0
