cmake_minimum_required(VERSION 3.20)
project(${MBR_TARGET}
    DESCRIPTION "Main Boot Record"
    LANGUAGES ASM_NASM
)

# MBR sources
set(SRCS "${PROJECT_SOURCE_DIR}/mbr.asm")

# LD Script
set(LD_SCRIPT "${PROJECT_SOURCE_DIR}/mbr.ld")

# Output symbols map
set(SYM_MAP "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

# Compile options
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
set_source_files_properties(${SRCS} PROPERTIES
    LANGUAGE ASM_NASM
    COMPILE_OPTIONS "${ASM_OPTIONS}"
)

# Add MBR target
add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL ${SRCS})

# Configure linking
target_link_options(${PROJECT_NAME} PRIVATE ${LINK_OPTIONS})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_DEPENDS ${LD_SCRIPT}
    LINK_FLAGS "-Xlinker --oformat=binary -Xlinker -Map=${SYM_MAP} -T ${LD_SCRIPT}"
)
