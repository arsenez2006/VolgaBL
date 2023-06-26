cmake_minimum_required(VERSION 3.20)
include(cmake/toolchain.cmake)
include(cmake/flags.cmake)
include(cmake/create_image.cmake)
project(VolgaOS ASM)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)

find_dependencies()

create_image()

# Compile bootloader
if(VLGBL_LEGACY)
    add_subdirectory(bootloader/legacy)
endif()

# Target for buiilding docs
add_custom_target(docs
    COMMAND doxygen ${PROJECT_SOURCE_DIR}/Doxyfile
)
