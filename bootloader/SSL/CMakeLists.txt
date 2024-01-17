cmake_minimum_required(VERSION 3.20)
project(${SSL_TARGET}
    DESCRIPTION "Second Stage Loader"
    LANGUAGES C ASM_NASM
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
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG")
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

# Generate docs
if(BUILD_DOCS)
    set(DOXYGEN_OUTPUT_DIRECTORY ${OUTPUT_DOCS}/VolgaBL/SSL)
    set(DOXYGEN_PROJECT_NAME "SSL")
    set(DOXYGEN_PROJECT_BRIEF "Second Stage Loader")
    #set(DOXYGEN_PROJECT_NUMBER ${VLGBL_VERSION})
    set(DOXYGEN_MACRO_EXPANSION YES)
    set(DOXYGEN_EXPAND_ONLY_PREDEF NO)
    list(APPEND DOXYGEN_PREDEFINED
        "__attribute__(x)="
        "__bool_true_false_are_defined"
        "DOX_SKIP"
    )
    doxygen_add_docs(${SSL_DOCS})
endif(BUILD_DOCS)