cpu 386
bits 16
global __start

; -------------------------------------------------------------------------------------------------
; DEFINES
; -------------------------------------------------------------------------------------------------
%define MBR_ADDR        0x7C00
%define MBR_SEG         MBR_ADDR >> 4

%define SSL_ADDR        0x10000
%define SSL_SIZE        64 * 1024
%define SSL_SEG         SSL_ADDR >> 4
%define SSL_SECTORS     SSL_SIZE / 512 - 1  ; FIXME: Some BIOSes can't read more than 127 sectors

%define BUFF_ADDR       SSL_ADDR
%define BUFF_SEG        BUFF_ADDR >> 4

%define STACK_BOT_ADDR  0x500
%define STACK_TOP_ADDR  MBR_ADDR - 16
%define STACK_SEG       STACK_BOT_ADDR >> 4
%define STACK_INIT      STACK_TOP_ADDR - STACK_BOT_ADDR

%define SECTOR_SIZE     512

; -------------------------------------------------------------------------------------------------
; MBR ERROR CODES:
;   1 - BIOS does not suppport int 13h extensions
;   2 - Boot drive has invalid sector size
;   3 - Failed to read GPT header
;   4 - Drive is not GPT
;   5 - Failed to read partition table
;   6 - Failed to find Second Stage Loader
;   7 - Failed to load Second Stage Loader
; -------------------------------------------------------------------------------------------------

; -------------------------------------------------------------------------------------------------
; MBR ENTRY
; -------------------------------------------------------------------------------------------------
section .text
__start:
    ; Initialize segments
    cli
    jmp MBR_SEG:.cs
.cs:
    mov ax, MBR_SEG
    mov ds, ax
    mov ax, BUFF_SEG
    mov es, ax
    mov ax, STACK_SEG
    mov ss, ax
    mov sp, STACK_INIT
    mov byte [drive_number], dl ; Save drive number
    sti

    ; Print loading message
    mov si, msg_loading
    call print

    ; Check int 13h extensions
    mov ah, 0x41
    mov bx, 0x55AA
    int 0x13
    mov al, '1'
    jc near .fail
    cmp bx, 0xAA55
    jne near .fail

    ; Check sector size
    mov byte [es:0x00], 26  ; Result buffer size (Excludes EDD)
    push ds
    mov ax, es
    mov ds, ax
    xor si, si  ; DS:SI = Result buffer
    mov ah, 0x48
    int 0x13
    pop ds
    mov al, '2'
    jc near .fail
    cmp word [es:0x18], SECTOR_SIZE
    jne near .fail

    ; Read GPT Header
    call read_drive
    mov al, '3'
    jc near .fail

    ; Check GPT Signature
    mov si, gpt_sig
    xor di, di
    mov cx, 8
    call memcmp
    mov al, '4'
    jc near .fail

    ; Get Number of Partition Entries
    mov eax, dword [es:0x50]
    push eax
    ; Get Size of each entry
    mov ebx, dword [es:0x54]
    push ebx
    ; Calculate size of Partition table
    mul ebx

    ; Calculate size of Partition table in sectors
    mov ebx, 512
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
    mov al, '5'
    jc short .fail

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
    jnc short .found

.next:
    add edx, ebp
    loop .loop
    mov al, '6'
    jmp short .fail
.found:

    mov ebp, edx

    ; Get SSL Start LBA (EBX:EAX)
    mov eax, dword [es:(ebp + 0x20 + 0)]
    mov ebx, dword [es:(ebp + 0x20 + 4)]

    ; Update DAP
    mov dword [DAP.lba_low], eax
    mov dword [DAP.lba_high], ebx
    mov word [DAP.sectors], SSL_SECTORS

    ; Load SSL to memory
    call read_drive
    mov al, '7'
    jc short .fail

    ; Far jump to SSL
    jmp SSL_SEG:0x0000

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

; print
; Prints string to terminal, using BIOS
; INPUT: DS:SI - Pointer to null-terminated string
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

; read_drive
; Reads sectors from saved drive, using DAP
; INPUT: Filled DAP
; OUTPUT: CF is set on error, clear on success
; LIMITATIONS: 
;   - Maximum buffer size is 64KB (1 real mode segment)
;   - Some BIOSes can't read more than 127 sectors
read_drive:
    mov ah, 0x42
    mov dl, byte [drive_number]
    mov si, DAP
    int 0x13
    ret

; memcmp
; Compares data
; INPUT:
;   - DS:SI - pointer to the first block of data
;   - ES:DI - pointer to the second block of data
;   - CX - number of bytes to compare
; OUTPUT: CF is set on mismatch, clear if equal
; Saves all registers, except FLAGS
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

; Saved drive number
drive_number db 0x00

; Messages
msg_loading db"Loading VolgaOS...",13,10,0
msg_fail db" - MBR fail.",0

; GPT signature
gpt_sig db"EFI PART"

; Second Stage Loader GPT partition type
ssl_part_type db 0x53,0xE6,0x86,0xC5,0x91,0x79,0x47,0x49,0xAC,0x24,0x75,0xF8,0xCF,0xF9,0x94,0x5C ; C586E653-7991-4947-AC24-75F8CFF9945C

; Disk Address Packet used for read_drive procedure
DAP:
.size:      db 0x10         ; Always 16
.rsv:       db 0x00         ; Always 0
.sectors:   dw 0x0001       ; first 8 bits - number of sectors to read, 9 - 16 must be zeroed
.offset:    dw 0x0000       ; Buffer offset
.segment:   dw 0x1000       ; Buffer segment
.lba_low:   dd 0x00000001   ; LBA of the disk (low 32 bits)
.lba_high:  dd 0x00000000   ; LBA of the disk (high 32 bits)
