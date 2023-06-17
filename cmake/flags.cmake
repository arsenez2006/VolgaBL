# Flags for 8086 target
list(APPEND C_8086_DIALECT 
    "-Wall"
    "-Wpedantic" 
    "-Wno-long-long"
    "-ansi" 
    "-fno-builtin" 
    "-ffreestanding" 
)
list(APPEND C_8086_OPTIMIZATION 
    ""
)
list(APPEND C_8086_INSTRUMENTATION 
    "-fno-stack-protector"
)
list(APPEND C_8086_GENERATION 
    "-freg-struct-return"
    "-fverbose-asm"
    "-fno-pic"
    "-fno-pie"
    "-fleading-underscore"
    "-fno-asynchronous-unwind-tables"
    "-fno-common"
)
list(APPEND C_8086_x86
    "-march=i386"
    "-mregparm=3"
    "-mpreferred-stack-boundary=2"
    "-mabi=sysv"
    "-mgeneral-regs-only"
    "-m16"
)
set(C_8086_FLAGS ${C_8086_DIALECT} ${C_8086_OPTIMIZATION} ${C_8086_INSTRUMENTATION} ${C_8086_GENERATION} ${C_8086_x86})
set(ASM_8086_FLAGS "-Ox -f elf32")
list(APPEND LINK_8086_FLAGS 
    ${C_8086_FLAGS}
    "-nostartfiles"
    "-nodefaultlibs"
    "-nolibc"
    "-nostdlib"
    "-s"
    "-static"
)
