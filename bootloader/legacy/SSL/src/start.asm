cpu 386
bits 16

global __start
global _drive_number
global _gdtr

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

section .data
_gdtr:
.size:      dw gdt.end - gdt.begin - 1
.offset:    dd gdt + 0x8000

gdt:
.begin:
.null:      dq 0x0000000000000000
.code:      dq 0x00CF9A000000FFFF
.data:      dq 0x00CF92000000FFFF
.end:

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

    ; ; Save real mode segment
    ; push ds
    ; push es

    ; ; Load GDT to enter Unreal mode
    ; lgdt [_gdtr]

    ; ; Enter Protected mode
    ; mov eax, cr0
    ; or al, 1
    ; mov cr0, eax
    ; jmp short $+2

    ; ; Load data descriptor cache
    ; mov bx, 0x10
    ; mov ds, bx
    ; mov es, bx
    
    ; ; Enter unreal mode
    ; and al, 0xFE
    ; mov cr0, eax
    ; ; Restore real mode segments
    ; pop es
    ; pop ds
    
    sti

    ; Call C code
    call _ssl_entry

.halt:
    hlt
    jmp short .halt
