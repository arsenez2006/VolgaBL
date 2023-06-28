cmake_minimum_required(VERSION 3.20)
project(${SSL_TARGET}
    LANGUAGES C ASM_NASM
    DESCRIPTION "Second Stage Loader"
)

# SSL Sources
file(GLOB_RECURSE C_SRCS "src/*.c")
file(GLOB_RECURSE ASM_SRCS "src/*.asm")
file(GLOB_RECURSE HDRS "include/*.h")
list(APPEND SRCS ${C_SRCS} ${ASM_SRCS})

# LD Script
set(LD_SCRIPT "${PROJECT_SOURCE_DIR}/SSL.ld")

# Output symbols map
set(SYM_MAP "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

# Compile options
list(APPEND C_OPTIONS
    ${C_DIALECT}
    ${C_OPTIMIZATION}
    ${C_INSTRUMENTATION}
    ${C_GENERATION}
    ${C_x86_16}
)
list(APPEND ASM_OPTIONS
    ${ASM_OPTIMIZATION}
    ${ASM_GENERATION}
)

# Link options
list(APPEND LINK_OPTIONS
    ${LINK_FLAGS}
    "-m16"
)

# Configure Sources
set_source_files_properties(${C_SRCS} PROPERTIES
    LANGUAGE C
    COMPILE_OPTIONS "${C_OPTIONS}"
)
set_source_files_properties(${ASM_SRCS} PROPERTIES
    LANGUAGE ASM_NASM
    COMPILE_OPTIONS "${ASM_OPTIONS}"
)

# Add SSL target
add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL ${SRCS} ${HDRS})
target_include_directories(${PROJECT_NAME} PRIVATE "include")

# Configure linking
target_link_options(${PROJECT_NAME} PRIVATE ${LINK_OPTIONS})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_DEPENDS ${LD_SCRIPT}
    LINK_FLAGS "-Xlinker --oformat=binary -Xlinker -Map=${SYM_MAP} -T ${LD_SCRIPT}"
)
