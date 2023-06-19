OUTPUT(binary)
ENTRY(__bootstrap)
MEMORY
{
    SSL_segment (rwx)   : ORIGIN = 0x00000000, LENGTH = 0x00010000
}
SECTIONS
{
    .text :
    {
        BYTE(0xE9);
        SHORT(__bootstrap - 3);
        *(.text)
    } > SSL_segment
    .rodata :
    {
        *(.rodata*)
    } > SSL_segment
    .data :
    {
        *(.data)
    } > SSL_segment
    .bss (NOLOAD) :
    {
        *(.bss)
    } > SSL_segment
    /DISCARD/ :
    {
        *(.note*)
        *(.comment*)
    }
}
