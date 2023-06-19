OUTPUT(binary)
ENTRY(__bootstrap)
MEMORY
{
    TSL_segment (rwx)   : ORIGIN = 0x00020000, LENGTH = 0x00010000
}
SECTIONS
{
    .text :
    {
        BYTE(0xE9);
        LONG(__bootstrap - 5);
        *(.text)
    } > TSL_segment
    .rodata :
    {
        *(.rodata)
    } > TSL_segment
    .data :
    {
        *(.data)
    } > TSL_segment
    .bss (NOLOAD) :
    {
        *(.bss)
    } > TSL_segment
    /DISCARD/ :
    {
        *(.note*)
        *(.comment*)
    }
}