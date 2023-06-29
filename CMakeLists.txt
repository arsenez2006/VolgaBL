cmake_minimum_required(VERSION 3.20)
include(cmake/toolchain.cmake)
include(cmake/create_image.cmake)
enable_language(C ASM_NASM)
project(VolgaOS)

include(cmake/options.cmake)
include(cmake/dependencies.cmake)

find_dependencies()

create_image()

# Compile bootloader
add_subdirectory(bootloader)

# Target for buiilding docs
add_custom_target(docs
    DEPENDS VLGBL_docs
)

add_custom_target(${PROJECT_NAME} ALL
    DEPENDS VLGBL
)
