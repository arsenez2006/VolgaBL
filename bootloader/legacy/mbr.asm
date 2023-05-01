cpu 386
bits 16
global __start
; -------------------------------------------------------------------------------------------------
; MBR ENTRY
; -------------------------------------------------------------------------------------------------
section .text
__start:
    ; Initialize segments
    cli
    jmp 0x07C0:.cs
.cs:
    mov ax, 0x07C0
    mov ds, ax
    mov ax, 0x0800
    mov es, ax
    mov ax, 0x0050
    mov ss, ax
    mov sp, 0x76F0
    mov byte [drive_number], dl
    sti

    ; Print loading message
    mov si, msg_loading
    call print

    ; Read GPT Header
    call read_drive
    mov al, '1'
    jc near .fail

    ; Check GPT Signature
    mov si, gpt_sig
    mov di, 0x00
    mov cx, 8
    call memcmp
    mov al, '2'
    jc near .fail

    ; Get Number of Partition Entries
    mov eax, dword [es:0x50]
    push eax
    ; Get Size of each entry
    mov ebx, dword [es:0x54]
    push ebx
    ; Calculate size of Partition table
    mul ebx

    ; Get drive parameteres
    pusha
    mov ah, 0x48
    mov dl, byte [drive_number]
    mov si, (0x8000 - 0x7C00)
    mov word [es:0x00], 0x1E
    int 0x13
    mov al, '3'
    jc near .fail
    popa

    ; Get sector size
    movzx ebx, word [es:0x18]
    ; Calculate size of Partition table in sectors
    div ebx
    or edx, edx
    jz short .skip_inc
    inc eax
.skip_inc:

    ; Update DAP
    mov word [DAP.sectors], ax
    mov eax, dword [es:(0x48 + 0)]
    mov dword [DAP.lba_low], eax
    mov eax, dword [es:(0x48 + 4)]
    mov dword [DAP.lba_high], eax
    
    ; Read Partition table
    call read_drive
    mov al, '4'
    jc near .fail

    ; Find SSL Partition header
    pop ebp         ; Entry size
    pop ecx         ; Entries count
    mov edx, 0x00   ; Start offset

.loop:
    ; Check partiotion type
    push cx
    mov si, ssl_part_type
    mov di, dx
    mov cx, 16
    call memcmp
    pop cx
    jc short .next

    ; Check partiotion name
    push cx
    mov si, ssl_part_name
    add di, 0x38
    mov cx, 10
    call memcmp
    pop cx
    jnc short .found

.next:
    add edx, ebp
    loop .loop
    mov al, '5'
    jmp near .fail
.found:

    mov ebp, edx

    ; Get SSL Start LBA (EBX:EAX)
    mov eax, dword [es:(ebp + 0x20 + 0)]
    mov ebx, dword [es:(ebp + 0x20 + 4)]

    ; Get SSL End LBA (EDX:ECX)
    mov ecx, dword [es:(ebp + 0x28 + 0)]
    mov edx, dword [es:(ebp + 0x28 + 4)]

    ; Calculate SSL Sectors count (EDX:ECX)
    sub ecx, eax
    sbb edx, ebx
    inc ecx

    ; Update DAP
    mov dword [DAP.lba_low], eax
    mov dword [DAP.lba_high], ebx
    mov word [DAP.sectors], cx

    ; Load SSL to memory
    call read_drive
    mov al, '6'
    jc near .fail

    ; Far jump to SSL
    jmp 0x0800:0x0000

.halt:
    hlt
    jmp short .halt
.fail:
    xor bx, bx
    mov ah, 0x0E
    int 0x10
    mov si, msg_fail
    call print
    jmp short .halt

; -------------------------------------------------------------------------------------------------
; PROCEDURES
; -------------------------------------------------------------------------------------------------
print:
    xor bx, bx
    mov ah, 0x0E
.loop:
    lodsb
    or al, al
    jz short .ret
    int 0x10
    jmp short .loop
.ret:
    ret

read_drive:
    mov ah, 0x42
    mov dl, byte [drive_number]
    mov si, DAP
    int 0x13
    ret

memcmp:
    pusha
.loop:
    lodsb
    cmp al, byte [es:di]
    jne short .fail
    inc di
    loop .loop
.success:
    clc
    jmp short .ret
.fail:
    stc
.ret:
    popa
    ret

; -------------------------------------------------------------------------------------------------
; DATA
; -------------------------------------------------------------------------------------------------
section .data
drive_number db 0x00
msg_loading db"Loading VolgaOS...",13,10,0
msg_fail db" - MBR fail.",0
gpt_sig db"EFI PART"
ssl_part_type db"Hah!IdontNeedEFI"
ssl_part_name db'V',0,'L',0,'G',0,'B',0,'L',0
DAP:
.size:      db 0x10
.rsv:       db 0x00
.sectors:   dw 0x0001
.offset:    dw 0x0000
.segment:   dw 0x0800
.lba_low:   dd 0x00000001
.lba_high:  dd 0x00000000
