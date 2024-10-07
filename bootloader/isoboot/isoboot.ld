OUTPUT(binary)
MEMORY
{
  segment (rwx) : org = 0, len = 64k
}

SECTIONS
{
  .text.pre : {
    *(.text.pre)
  } > segment
}
