cpu 386
bits 16

global __start
global _drive_number
global __heap
global __heap_size

extern _ssl_entry
extern _printf

section .bss
; Allocate stack
stack:
align 16
.top:
    resb 1024 * 4
.bottom:

; Allocate heap
__heap:
align 16
.start:
    resb 1024 * 32
.end:

; Drive number
_drive_number:
    resb 1

section .data
__heap_size:
    dw __heap.end - __heap.start
err_msg db"VLGBL Second Stage Loader Failed, aborting.",0

section .text
__start:
    ; Initialize segments
    cli
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, stack.bottom

    ; Save drive number
    mov byte [_drive_number], dl

    sti
    
    ; Call C code
    call _ssl_entry
    cmp ax, 0
    jz short .enter_pm
    push err_msg
    call _printf
    jmp short .halt

    ; Enter Protected mode
.enter_pm:
    cli
    ; Disable PIC
    mov al, 0xff
    out 0xa1, al
    out 0x21, al
    ; Set CR0 PE bit
    mov eax, cr0
    or al, 1
    mov cr0, eax
    ; Clear instruction pipeline
    jmp short $+2

    ; Set 32bit segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    ; Fix stack
    and esp, 0xFFFF
    add esp, 0x10000

    ; I spent two weeks to realize that CPU still uses 16bit offset for jump, so I'll make a jump stub in the beggining of memory
    mov byte [0x00], 0xE9
    mov dword [0x01], 0x20000

    ; Jump to the Third Stage Loader
    jmp 0x08:0x0000
bits 16
.halt:
    hlt
    jmp short .halt
