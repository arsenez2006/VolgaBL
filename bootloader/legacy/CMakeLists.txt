cmake_minimum_required(VERSION 3.20)
project(bl_legacy ASM C)

# Variables
set(MBR_TARGET ${PROJECT_NAME}_mbr)
set(SSL_TARGET ${PROJECT_NAME}_ssl)
set(TSL_TARGET ${PROJECT_NAME}_tsl)

# Compile MBR
add_executable(${MBR_TARGET} EXCLUDE_FROM_ALL mbr.asm)

# Compile SSL
file(GLOB_RECURSE SSL_SRCS "SSL/src/*.c" "SSL/src/*.asm")
file(GLOB_RECURSE SSL_HDRS "SSL/include/*.h")
add_executable(${SSL_TARGET} EXCLUDE_FROM_ALL ${SSL_SRCS} ${SSL_HDRS})
target_include_directories(${SSL_TARGET} PRIVATE "SSL/include")

# # Compile TSL
# file(GLOB_RECURSE TSL_SRCS "TSL/src/*.c" "TSL/src/*.asm")
# file(GLOB_RECURSE TSL_HDRS "TSL/include/*.h")
# add_executable(${TSL_TARGET} EXCLUDE_FROM_ALL ${TSL_SRCS} ${TSL_HDRS})
# target_include_directories(${TSL_TARGET} PRIVATE "TSL/include") 

# Write bootloader to ISO
add_custom_target(${PROJECT_NAME} ALL
    COMMAND ${DD} if=$<TARGET_FILE:${MBR_TARGET}> of=${OUT_ISO} bs=${OUT_ISO_MBR_BS} count=${OUT_ISO_MBR_COUNT} seek=${OUT_ISO_MBR_OFF} conv=notrunc
    COMMAND ${DD} if=$<TARGET_FILE:${SSL_TARGET}> of=${OUT_ISO} bs=${OUT_ISO_SSL_BS} count=${OUT_ISO_SSL_COUNT} seek=${OUT_ISO_SSL_OFF} conv=notrunc
    DEPENDS ${MBR_TARGET} ${SSL_TARGET}
)
