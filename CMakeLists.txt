cmake_minimum_required(VERSION 3.20)

# Include CMake scripts
include(cmake/options.cmake)
include(cmake/dependencies.cmake)
include(cmake/flags.cmake)
include(cmake/toolchain.cmake)

project(VolgaBL
    DESCRIPTION "VolgaOS legacy bootloader"
    HOMEPAGE_URL "https://github.com/arsenez2006/VolgaBL"
    LANGUAGES C ASM_NASM
)

# Find dependencies
find_dependencies()

# Compile bootloader
add_subdirectory(bootloader)

if(BUILD_DOCS)
    set(DEPS bootloader bootloader_docs)
else()
    set(DEPS bootloader)
endif()

# Copy target files
add_custom_target(${PROJECT_NAME} ALL
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${MBR_TARGET}> ${OUTPUT}/${MBR_TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${SSL_TARGET}> ${OUTPUT}/${SSL_TARGET}
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TSL_TARGET}> ${OUTPUT}/${TSL_TARGET}
    DEPENDS ${DEPS}
)
