list(APPEND C_DIALECT 
    "-Wall"
    "-Wpedantic" 
    "-Wno-long-long"
    "-ansi" 
    "-fno-builtin" 
    "-ffreestanding" 
)
list(APPEND C_OPTIMIZATION 
    "-Os"
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

list(APPEND ASM_OPTIMIZATION
    "-Ox"
)

list(APPEND ASM_GENERATION
    "-felf32"
)

list(APPEND LINK_FLAGS 
    "-nostartfiles"
    "-nodefaultlibs"
    "-nolibc"
    "-nostdlib"
    "-s"
    "-static"
)
