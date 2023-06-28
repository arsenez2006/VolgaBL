cmake_minimum_required(VERSION 3.20)
include(cmake/flags.cmake)
project(VLGBL_legacy
    DESCRIPTION "VLGBL Legacy bootloader"
)

# Targets
set(MBR_TARGET ${PROJECT_NAME}_mbr)
set(SSL_TARGET ${PROJECT_NAME}_ssl)
set(TSL_TARGET ${PROJECT_NAME}_tsl)

# Compile MBR
add_subdirectory(MBR)

# Compile SSL
add_subdirectory(SSL)

# Compile TSL
add_subdirectory(TSL)

# Write bootloader to image
add_custom_target(${PROJECT_NAME}
    COMMAND ${DD} if=$<TARGET_FILE:${MBR_TARGET}> of=${OUTPUT_IMAGE} bs=${OUTPUT_MBR_BS} count=${OUTPUT_MBR_SIZE} seek=${OUTPUT_MBR_OFFSET} conv=notrunc
    COMMAND ${DD} if=$<TARGET_FILE:${SSL_TARGET}> of=${OUTPUT_IMAGE} bs=${OUTPUT_SSL_BS} count=${OUTPUT_SSL_SIZE} seek=${OUTPUT_SSL_OFFSET} conv=notrunc
    COMMAND ${DD} if=$<TARGET_FILE:${TSL_TARGET}> of=${OUTPUT_IMAGE} bs=${OUTPUT_TSL_BS} count=${OUTPUT_TSL_SIZE} seek=${OUTPUT_TSL_OFFSET} conv=notrunc
    DEPENDS ${MBR_TARGET} ${SSL_TARGET} ${TSL_TARGET}
)

set(BOOTLOADER_LEGACY_DEPENDENCY ${PROJECT_NAME} PARENT_SCOPE)
