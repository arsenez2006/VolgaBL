OUTPUT(binary)
MEMORY
{
    MBR_code (rwx)  : ORIGIN = 0x00000000, LENGTH = 0x000001BE
    MBR_bss (rw)    : ORIGIN = 0x000001BE, LENGTH = 0x00000042
}

SECTIONS
{
    .text :
    {
        *(.text)
        *(.data)
    } > MBR_code
    .bss (NOLOAD) :
    {
        *(.bss)
    } > MBR_bss
}
