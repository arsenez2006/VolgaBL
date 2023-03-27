cpu 386
bits 16

global _get_ds
global _load_GDT
global _enter_unreal
global __add_dwords
global __sub_dwords

section .text
_get_ds:
    mov ax, ds
    ret

_load_GDT:
    push bp
    mov bp, sp

    lgdt [bp + 4]

    mov sp, bp
    pop bp
    ret

_enter_unreal:
    push bp
    mov bp, sp
    push eax
    push bx

    ; Save real mode segments and disable interrupts
    cli
    push ds
    push es

    ; Enter Protected mode
    mov eax, cr0
    or al, 1
    mov cr0, eax
    
    ; Clear CPU intstruction cache
    jmp short $+2
    
    ; Load data descriptor
    mov bx, word [bp + 4]
    mov ds, bx
    mov es, bx

    ; Enter Unreal mode by disabling Protected mode
    and al, 0xFE
    mov cr0, eax

    ; Restore real mode segments and enable interrupts
    pop es
    pop ds
    sti

    pop bx
    pop eax
    mov sp, bp
    pop bp
    ret

__add_dwords:
    push bp
    mov bp, sp
    push si
    push eax

    mov eax, dword [bp + 4]
    add eax, dword [bp + 8]
    mov si, word [bp + 12]
    mov dword [si], eax
    
    pop eax
    pop si
    mov sp, bp
    pop bp
    ret

__sub_dwords:
    push bp
    mov bp, sp
    push si
    push eax

    mov eax, dword [bp + 4]
    sub eax, dword [bp + 8]
    mov si, word [bp + 12]
    mov dword [si], eax

    pop si
    pop eax
    mov sp, bp
    pop bp
    ret
