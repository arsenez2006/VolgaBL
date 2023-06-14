set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR 8086)

# Assembler
find_program(CMAKE_ASM_COMPILER nasm REQUIRED)
set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <FLAGS> -o <OBJECT> <SOURCE>")
set(CMAKE_ASM_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> -o <TARGET> <OBJECTS>")
set(CMAKE_ASM_FLAGS "-Ox")

# C Compiler
find_program(CMAKE_C_COMPILER gcc REQUIRED)
set(CMAKE_C_COMPILE_OBJECT "<CMAKE_C_COMPILER> <INCLUDES> <FLAGS> -o <OBJECT> -c <SOURCE>")
set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> -o <TARGET> <OBJECTS>")

# list(APPEND C_8086_DIALECT 
#     "-Wall"
#     "-Wpedantic" 
#     "-x c" 
#     "-ansi" 
#     "-fno-builtin" 
#     "-ffreestanding" 
# )
# set(C_8086_OPTIMIZATION "-Oz")
# set(C_8086_INSTRUMENTATION "")
# list(APPEND C_8086_GENERATION 
#     "-freg-struct-return"
#     "-fverbose-asm"
#     "-fno-plt"
#     "-fleading-underscore"
# )
# list(APPEND C_8086_x86
#     "-march=i386"
#     "-masm=intel"
#     "-mregparam=3"
#     "-mpreferred-stack-boundary=2"
#     "-mabi=sysv"
#     "-mgeneral-regs-only"
#     "-m16"
# )
# set(CMAKE_C_FLAGS ${C_8086_DIALECT} ${C_8086_OPTIMIZATION} ${C_8086_INSTRUMENTATION} ${C_8086_GENERATION} ${C_8086_x86})

# set(CMAKE_C_COMPILER_WORKS TRUE)

# Linker
find_program(CMAKE_LINKER gcc REQUIRED)
set(CMAKE_EXE_LINKER_FLAGS "-march=i386 -m16 -nostartfiles -nodefaultlibs -nolibc -nostdlib -e __bootstrap -s -static -Xlinker \"--oformat=binary\"")
