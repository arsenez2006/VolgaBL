cpu 386
bits 16
global _bios_putch

section .text
_bios_putch:
    push bp
    mov bp, sp
    push bx

    mov al, byte [bp + 4]
    mov ah, 0x0E
    xor bx, bx
    int 0x10

    pop bx
    mov sp, bp
    pop bp
    ret
