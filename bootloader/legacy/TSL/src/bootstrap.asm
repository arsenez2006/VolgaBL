cpu 686
bits 32

global __bootstrap

extern _tsl_entry

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

; -------------------------------------------------------------------------------------------------
; TEXT
; -------------------------------------------------------------------------------------------------
section .text
__bootstrap:
    ; Setup stack
    cld
    mov esp, stack.top

    ; Call C code
    call _tsl_entry
