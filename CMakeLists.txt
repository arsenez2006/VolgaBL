cmake_minimum_required(VERSION 3.20)
include(cmake/toolchains/8086.cmake)
project(VolgaOS ASM)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)

find_dependencies()

# ISO variables
set(OUT_ISO ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.iso)
set(OUT_ISO_MBR_BS 1)
set(OUT_ISO_MBR_OFF 0)
set(OUT_ISO_MBR_COUNT 446)
set(OUT_ISO_SSL_BS 512)
set(OUT_ISO_SSL_OFF 2048)
set(OUT_ISO_SSL_COUNT 128)

if(NOT EXISTS ${OUT_ISO})
    exec_program(${DD}
        ARGS if=/dev/zero of=${OUT_ISO} bs=$[1024 * 1024] count=$[8 * 1024]
    )
    exec_program(${SFDISK}
        ARGS ${OUT_ISO} < ${PROJECT_SOURCE_DIR}/disk_layout.map
    )
endif()

# Compile bootloader
add_subdirectory(bootloader/legacy)
