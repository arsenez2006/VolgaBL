cpu 386
bits 16
global _bios_putch
global _bios_read_drive
global _bios_get_drive_parameteres
global _bios_get_e820
global _bios_serial_init
global _bios_serial_putch

section .text
_bios_putch:
    push bp
    mov bp, sp
    push ax
    push bx

    mov al, byte [bp + 4]
    mov ah, 0x0E
    xor bx, bx
    int 0x10

    pop bx
    pop ax
    mov sp, bp
    pop bp
    ret

extern _drive_number
_bios_read_drive:
    push bp
    mov bp, sp
    push si
    push dx

    mov ah, 0x42
    mov dl, byte [_drive_number]
    mov si, word [bp + 4]

    int 0x13
    jnc short .success

.fail:
    mov ax, 0
    jmp short .ret
.success:
    mov ax, 1
.ret:
    pop dx
    pop si
    mov sp, bp
    pop bp
    ret

_bios_get_drive_parameteres:
    push bp
    mov bp, sp
    push si
    push dx

    mov ah, 0x48
    mov dl, byte [_drive_number]
    mov si, word [bp + 4]
    int 0x13

    jnc short .success

.fail:
    mov ax, 0
    jmp short .ret
.success:
    mov ax, 1
.ret:
    pop dx
    pop si
    mov sp, bp
    pop bp
    ret

_bios_get_e820:
    push bp
    mov bp, sp
    push si
    push ebx
    push ecx
    push edx

    mov eax, 0xE820
    mov edx, 0x534D4150
    mov si, word [bp + 4]
    mov ebx, dword [si]
    movzx ecx, word [bp + 6]
    mov di, [bp + 8]

    int 0x15
    
    mov dword [si], ebx
    cmp eax, 0x534D4150
    mov ax, 0
    jne short .ret
    mov ax, 1
    
.ret:
    pop edx
    pop ecx
    pop ebx
    pop si
    mov sp, bp
    pop bp
    ret

_bios_serial_init:
    push bp
    mov bp, sp
    push dx

    mov al, 11100011b
    mov ah, 0x00
    mov dx, 0x00
    int 0x14

    jnc short .success

.fail:
    mov ax, 0x00
    jmp short .ret
.success:
    mov ax, 0x01
.ret:
    pop dx
    mov sp, bp
    pop bp
    ret

_bios_serial_putch:
    push bp
    mov bp, sp
    push ax
    push dx

    mov al, byte [bp + 4]
    mov ah, 0x01
    mov dx, 0x00
    int 0x14

    pop dx
    pop ax
    mov sp, bp
    pop bp
    ret
