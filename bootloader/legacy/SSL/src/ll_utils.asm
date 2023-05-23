cpu 386
bits 16

global _get_ds
global _enable_A20
global _load_GDT
global _enter_unreal
global _init_crc32
global _crc32
global __add_dwords
global __sub_dwords
global __mul_dwords
global __div_dwords
global __mod_dwords
global __shl_dword
global __shr_dword

section .bss
crc32table:
    resd 256

section .text
_get_ds:
    mov ax, ds
    ret

check_A20:
    pushf
    push ds
    push es
    push di
    push si
 
    cli
 
    xor ax, ax ; ax = 0
    mov es, ax
 
    not ax ; ax = 0xFFFF
    mov ds, ax
 
    mov di, 0x0500
    mov si, 0x0510
 
    mov al, byte [es:di]
    push ax
 
    mov al, byte [ds:si]
    push ax
 
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
 
    cmp byte [es:di], 0xFF
 
    pop ax
    mov byte [ds:si], al
 
    pop ax
    mov byte [es:di], al
 
    mov ax, 0
    je .ret
 
    mov ax, 1
 
.ret:
    pop si
    pop di
    pop es
    pop ds
    popf

    ret

_enable_A20:
    ; Check if A20 is already enabled
    call check_A20
    or ax, ax
    jnz near .ret  ; A20 enabled

    ; Try BIOS method
    jmp .kb_success
    mov ax, 0x2403
    int 0x15
    jb short .bios_fail
    cmp ah, 0
    jnz short .bios_fail

    mov ax, 0x2402
    int 0x15
    jb short .bios_fail
    cmp ah, 0
    jnz short .bios_fail

    cmp al, 1
    jz short .bios_success

    mov ax, 0x2401
    int 0x15
    jb short .bios_fail
    cmp ah, 0
    jnz short .bios_fail

.bios_success:
    call check_A20
    or ax, ax
    jnz short .ret  ; A20 enabled
.bios_fail:
    ; Try keyboard controller method
    cli

    call .kb_wait
    mov al, 0xAD
    out 0x64, al

    call .kb_wait
    mov al, 0xD0
    out 0x64, al

    call .kb_wait2
    in al, 0x60
    push ax

    call .kb_wait
    mov al, 0xD1
    out 0x64, al

    call .kb_wait
    pop ax
    or al, 2
    out 0x60, al

    call .kb_wait
    mov al, 0xAE
    out 0x64, al

    call .kb_wait
    sti
    jmp short .kb_success

.kb_wait:
    in al, 0x64
    test al, 2
    jnz short .kb_wait
    ret
.kb_wait2:
    in al, 0x64
    test al, 1
    jz short .kb_wait2
    ret

.kb_success:
    call check_A20
    or ax, ax
    jnz short .ret  ; A20 enabled

    ; Try Fast A20 method
    in al, 0x92
    or al, 2
    out 0x92, al

    call check_A20
.ret:
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

_init_crc32:
    push eax
    push ebx
    push ecx
    push edi
    
    mov edi, crc32table
    xor ebx, ebx
.calc_table:
    mov eax, ebx
    mov ecx, 8
.do_polynom:
    shr eax, 1
    jnc short .next
    xor eax, 0x0EDB88320
.next:
    loop .do_polynom

    stosd
    inc ebx
    cmp ebx, 256
    jb short .calc_table

    pop edi
    pop ecx
    pop ebx
    pop eax
    ret

_crc32:
    push bp
    mov bp, sp
    push si
    push eax
    push ebx
    push cx
    push edx

    mov edx, 0xFFFFFFFF
    mov si, word [bp + 4]
    mov cx, word [bp + 6]
    
    xor eax, eax
.calc_crc32:
    lodsb
    mov ebx, edx
    and ebx, 0x000000FF
    xor bl, al

    shl ebx, 2
    shr edx, 8
    and edx, 0x00FFFFFF
    xor edx, [crc32table + ebx]

    loop .calc_crc32

    mov eax, edx
    xor eax, 0xFFFFFFFF

    mov si, word [bp + 8]
    mov dword [si], eax

    pop edx
    pop cx
    pop ebx
    pop eax
    pop si
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

__mul_dwords:
    push bp
    mov bp, sp
    push eax
    push edx
    push si

    mov eax, dword [bp + 4]
    mul dword [bp + 8]
    mov si, word [bp + 12]
    mov dword [si], eax
    
    pop si
    pop edx
    pop eax
    mov sp, bp
    pop bp
    ret

__div_dwords:
    push bp
    mov bp, sp
    push eax
    push edx
    push si

    xor edx, edx
    mov eax, dword [bp + 4]
    div dword [bp + 8]
    mov si, word [bp + 12]
    mov dword [si], eax

    pop si
    pop edx
    pop eax
    mov sp, bp
    pop bp
    ret

__mod_dwords:
    push bp
    mov bp, sp
    push eax
    push edx
    push si

    xor edx, edx
    mov eax, dword [bp + 4]
    div dword [bp + 8]
    mov si, word [bp + 12]
    mov dword [si], edx

    pop si
    pop edx
    pop eax
    mov sp, bp
    pop bp
    ret

__shl_dword:
    push bp
    mov bp, sp
    push si
    push eax
    push cx

    mov eax, dword [bp + 4]
    mov cl, byte [bp + 8]
    shl eax, cl
    mov si, word [bp + 10]
    
    mov dword [si], eax
    
    pop cx
    pop eax
    pop si
    mov sp, bp
    pop bp
    ret

__shr_dword:
    push bp
    mov bp, sp
    push si
    push eax
    push cx

    mov eax, dword [bp + 4]
    mov cl, byte [bp + 8]
    shr eax, cl
    mov si, word [bp + 10]
    
    mov dword [si], eax
    
    pop cx
    pop eax
    pop si
    mov sp, bp
    pop bp
    ret

__add_qwords:
    push bp
    mov bp, sp
    push eax
    push ebx
    push ecx
    push edx
    push si

    mov eax, dword [bp + 4]
    mov ebx, dword [bp + 8]
    mov ecx, dword [bp + 12]
    mov edx, dword [bp + 16]

    add eax, ecx
    adc ebx, edx

    mov si, word [bp + 20]
    mov dword [si + 0], eax
    mov dword [si + 4], ebx

    pop si
    pop edx
    pop ecx
    pop ebx
    pop eax
    mov sp, bp
    pop bp
    ret
