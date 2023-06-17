cpu 386
bits 16

global __bootstrap
global __drive_number
global __heap
global __heap_size

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

; Allocate heap
__heap:
align 16
.begin:
    resb 1024 * 32
.end:


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
; Heap size
__heap_size:
    dw __heap.end - __heap.begin
