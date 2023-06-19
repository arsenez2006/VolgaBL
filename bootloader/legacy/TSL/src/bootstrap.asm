bits 32
global __bootstrap

__bootstrap:
    mov eax, 0xCCCCCCCC
.halt:
    hlt 
    jmp short .halt
