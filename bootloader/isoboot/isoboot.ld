OUTPUT(binary)
MEMORY
{
  preload (rwx) : org = 0, len = 2k
  extended (rwx) : org = 0x800, len = 62k
}

SECTIONS
{
  .text.pre : {
    *(.text.pre)
  } > preload
  .rodata.pre : {
    *(.rodata.pre)
  } > preload
  .data.pre : {
    *(.data.pre)
  }
  .text : {
  } > extended
}
