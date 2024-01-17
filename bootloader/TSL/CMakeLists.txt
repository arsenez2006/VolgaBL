cmake_minimum_required(VERSION 3.20)
project(${TSL_TARGET}
    DESCRIPTION "Third Stage Loader"
    LANGUAGES C ASM_NASM
)

# TSL Sources
file(GLOB_RECURSE C_SRCS "src/*.c")
file(GLOB_RECURSE ASM_SRCS "src/*.asm")
file(GLOB_RECURSE HDRS "include/*.h")
list(APPEND SRCS ${C_SRCS} ${ASM_SRCS})

# LD Script
set(LD_SCRIPT "${PROJECT_SOURCE_DIR}/TSL.ld")

# Output symbols map
set(SYM_MAP "${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map")

# Compile options
list(APPEND C_OPTIONS
    ${C_DIALECT}
    ${C_OPTIMIZATION}
    ${C_INSTRUMENTATION}
    ${C_GENERATION}
    ${C_x86_32}
)
list(APPEND ASM_OPTIONS
    ${ASM_OPTIMIZATION}
    ${ASM_GENERATION}
)

# Link options
list(APPEND LINK_OPTIONS
    ${LINK_FLAGS}
    "-m32"
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

# Add TSL target
add_executable(${PROJECT_NAME} EXCLUDE_FROM_ALL ${SRCS} ${HDRS})
target_include_directories(${PROJECT_NAME} PRIVATE "include")

# Configure linking
target_link_options(${PROJECT_NAME} PRIVATE ${LINK_OPTIONS})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_DEPENDS ${LD_SCRIPT}
    LINK_FLAGS "-Xlinker --oformat=binary -Xlinker -Map=${SYM_MAP} -T ${LD_SCRIPT}"
)

# Generate docs
if(BUILD_DOCS)
    set(DOXYGEN_OUTPUT_DIRECTORY ${OUTPUT_DOCS}/VolgaBL/TSL)
    set(DOXYGEN_PROJECT_NAME "TSL")
    set(DOXYGEN_PROJECT_BRIEF "Third Stage Loader")
    #set(DOXYGEN_PROJECT_NUMBER ${VLGBL_VERSION})
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_EXPAND_ONLY_PREDEF NO)
    list(APPEND DOXYGEN_PREDEFINED
        "__attribute__(x)="
        "__bool_true_false_are_defined"
        "DOX_SKIP"
    )
    doxygen_add_docs(${TSL_DOCS})
endif(BUILD_DOCS)
