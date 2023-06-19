list(APPEND C_DIALECT 
    "-Wall"
    "-Wpedantic" 
    "-Wno-long-long"
    "-ansi" 
    "-fno-builtin" 
    "-ffreestanding" 
)
list(APPEND C_OPTIMIZATION 
    ""
)
list(APPEND C_INSTRUMENTATION 
    "-fno-stack-protector"
)
list(APPEND C_GENERATION 
    "-freg-struct-return"
    "-fverbose-asm"
    "-fno-pic"
    "-fno-pie"
    "-fleading-underscore"
    "-fno-asynchronous-unwind-tables"
    "-fno-common"
)
list(APPEND C_x86_16
    "-march=i386"
    "-mregparm=3"
    "-mpreferred-stack-boundary=3"
    "-mabi=sysv"
    "-mgeneral-regs-only"
    "-m16"
)

list(APPEND C_x86_32
    "-march=i686"
    "-mregparm=3"
    "-mpreferred-stack-boundary=3"
    "-mabi=sysv"
    "-mgeneral-regs-only"
    "-m32"
)

list(APPEND LINK_FLAGS 
    "-nostartfiles"
    "-nodefaultlibs"
    "-nolibc"
    "-nostdlib"
    "-s"
    "-static"
)

# Flags for 16bit target
set(ASM_FLAGS_16 "-Ox -f elf32")
set(C_FLAGS_16 ${C_DIALECT} ${C_OPTIMIZATION} ${C_INSTRUMENTATION} ${C_GENERATION} ${C_x86_16})
set(LINK_FLAGS_16 ${C_FLAGS_16} ${LINK_FLAGS})

# Flags for 32bit target
set(ASM_FLAGS_32 "-Ox -f elf32")
set(C_FLAGS_32 ${C_DIALECT} ${C_OPTIMIZATION} ${C_INSTRUMENTATION} ${C_GENERATION} ${C_x86_32})
set(LINK_FLAGS_32 ${C_FLAGS_32} ${LINK_FLAGS})
