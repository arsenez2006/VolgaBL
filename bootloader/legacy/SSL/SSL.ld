OUTPUT(binary)
MEMORY
{
    SSL_segment (rwx)   : ORIGIN = 0x00000000, LENGTH = 0x00010000
}
SECTIONS
{
    .text :
    {
        *(.text)
    } > SSL_segment
    .rodata :
    {
        *(.rodata)
    } > SSL_segment
    .data :
    {
        *(.data)
    } > SSL_segment
    .bss (NOLOAD) :
    {
        *(.bss)
    } > SSL_segment
}
