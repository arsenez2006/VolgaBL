OUTPUT(binary)
ENTRY(_entry)
MEMORY
{
  code (rwx) : ORIGIN = 0x00001000, LENGTH = 0x000001B8
}

SECTIONS
{
  .text :
  {
    *(.text)
    *(.data)
  } > code
  /DISCARD/ :
  {
    *(.note*)
    *(.comment*)
  }
}
