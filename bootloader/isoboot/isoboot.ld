OUTPUT(binary)
MEMORY
{
  preload (rwx) : org = 0, len = 64k
}

SECTIONS
{
  .text.pre : {
    *(.text.pre)
  } > preload
  .rodata.pre : {
    *(.rodata.pre)
  }
}
