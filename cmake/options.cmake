# General configuration
set(OUTPUT "${CMAKE_BINARY_DIR}/out" CACHE PATH "Bootloader targets directory")
option(BUILD_DOCS "Build documentation (requires doxygen)" OFF)
set(OUTPUT_DOCS "${OUTPUT}/docs" CACHE PATH "Documentation directory")
