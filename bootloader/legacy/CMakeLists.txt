cmake_minimum_required(VERSION 3.20)
project(bl_legacy ASM C)

# Variables
set(MBR_TARGET ${PROJECT_NAME}_mbr)
set(SSL_TARGET ${PROJECT_NAME}_ssl)
set(TSL_TARGET ${PROJECT_NAME}_tsl)

# Compile MBR
add_executable(${MBR_TARGET} EXCLUDE_FROM_ALL mbr.asm)
set_source_files_properties(mbr.asm PROPERTIES 
    COMPILE_FLAGS "${ASM_FLAGS_16}"
)
set_target_properties(${MBR_TARGET} PROPERTIES
    LINK_DEPENDS ${PROJECT_SOURCE_DIR}/mbr.ld
    LINK_FLAGS "-Xlinker --oformat=binary -Xlinker -Map=${PROJECT_BINARY_DIR}/${MBR_TARGET}.map -T ${PROJECT_SOURCE_DIR}/mbr.ld"
)
target_link_options(${MBR_TARGET} PRIVATE ${LINK_FLAGS_16})

# Compile SSL
file(GLOB_RECURSE SSL_C_SRCS "SSL/src/*.c")
file(GLOB_RECURSE SSL_ASM_SRCS "SSL/src/*.asm")
file(GLOB_RECURSE SSL_HDRS "SSL/include/*.h")
list(APPEND SSL_SRCS ${SSL_C_SRCS} ${SSL_ASM_SRCS})
add_executable(${SSL_TARGET} EXCLUDE_FROM_ALL ${SSL_SRCS} ${SSL_HDRS})
set_source_files_properties(${SSL_ASM_SRCS} PROPERTIES 
    COMPILE_FLAGS "${ASM_FLAGS_16}"
)
set_source_files_properties(${SSL_C_SRCS} PROPERTIES
    COMPILE_OPTIONS "${C_FLAGS_16}"
)
target_include_directories(${SSL_TARGET} PRIVATE "SSL/include")
set_target_properties(${SSL_TARGET} PROPERTIES
    LINK_DEPENDS ${PROJECT_SOURCE_DIR}/SSL/SSL.ld
    LINK_FLAGS "-Xlinker --oformat=binary -Xlinker -Map=${PROJECT_BINARY_DIR}/${SSL_TARGET}.map -T ${PROJECT_SOURCE_DIR}/SSL/SSL.ld"
)
target_link_options(${SSL_TARGET} PRIVATE ${LINK_FLAGS_16})

# Compile TSL
file(GLOB_RECURSE TSL_C_SRCS "TSL/src/*.c")
file(GLOB_RECURSE TSL_ASM_SRCS "TSL/src/*.asm")
file(GLOB_RECURSE TSL_HDRS "TSL/include/*.h")
list(APPEND TSL_SRCS ${TSL_C_SRCS} ${TSL_ASM_SRCS})
add_executable(${TSL_TARGET} EXCLUDE_FROM_ALL ${TSL_SRCS} ${TSL_HDRS})
set_source_files_properties(${TSL_ASM_SRCS} PROPERTIES
    COMPILE_FLAGS "${ASM_FLAGS_32}"
)
set_source_files_properties(${TSL_C_SRCS} PROPERTIES
    COMPILE_OPTIONS "${C_FLAGS_32}"
)
target_include_directories(${TSL_TARGET} PRIVATE "TSL/include")
set_target_properties(${TSL_TARGET} PROPERTIES
    LINK_DEPENDS ${PROJECT_SOURCE_DIR}/TSL/TSL.ld
    LINK_FLAGS "-Xlinker --oformat=binary -Xlinker -Map=${PROJECT_BINARY_DIR}/${TSL_TARGET}.map -T ${PROJECT_SOURCE_DIR}/TSL/TSL.ld"
)
target_link_options(${TSL_TARGET} PRIVATE ${LINK_FLAGS_32})

# Write bootloader to ISO
add_custom_target(${PROJECT_NAME} ALL
    COMMAND ${DD} if=$<TARGET_FILE:${MBR_TARGET}> of=${OUTPUT_IMAGE} bs=${OUTPUT_MBR_BS} count=${OUTPUT_MBR_SIZE} seek=${OUTPUT_MBR_OFFSET} conv=notrunc
    COMMAND ${DD} if=$<TARGET_FILE:${SSL_TARGET}> of=${OUTPUT_IMAGE} bs=${OUTPUT_SSL_BS} count=${OUTPUT_SSL_SIZE} seek=${OUTPUT_SSL_OFFSET} conv=notrunc
    COMMAND ${DD} if=$<TARGET_FILE:${TSL_TARGET}> of=${OUTPUT_IMAGE} bs=${OUTPUT_TSL_BS} count=${OUTPUT_TSL_SIZE} seek=${OUTPUT_TSL_OFFSET} conv=notrunc
    DEPENDS ${MBR_TARGET} ${SSL_TARGET} ${TSL_TARGET}
)
