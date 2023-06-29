# Executables
set(DD_PATH "" CACHE FILEPATH "Full path to DD executable")
set(SFDISK_PATH "" CACHE FILEPATH "Full path to SFDISK executable")

# General configuration
option(BUILD_DOCS "Build documentation (requires doxygen)" OFF)

# Bootloader configuration
option(VLGBL_LEGACY "Build legacy bootloader" ON)
option(VLGBL_UEFI "Build UEFI bootloader" ON)

# Output image
set(OUTPUT_IMAGE "${CMAKE_BINARY_DIR}/VolgaOS.iso" CACHE FILEPATH "Path to output image")
