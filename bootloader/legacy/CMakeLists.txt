cmake_minimum_required(VERSION 3.20)
project(bl_legacy ASM)

# Variables
set(MBR_TARGET ${PROJECT_NAME}_mbr)

# Compile MBR
add_executable(${MBR_TARGET} EXCLUDE_FROM_ALL mbr.asm)

# Write bootloader to ISO
add_custom_target(${PROJECT_NAME} ALL
    COMMAND ${DD} if=$<TARGET_FILE:${MBR_TARGET}> of=${OUT_ISO} bs=${OUT_ISO_MBR_BS} count=${OUT_ISO_MBR_COUNT} seek=${OUT_ISO_MBR_OFF} conv=notrunc
    DEPENDS ${MBR_TARGET}
)
