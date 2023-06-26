# Executables
option(DD_PATH "Full path to DD executable.")
option(SFDISK_PATH "Full path to SFDISK executable")

# Bootloader configuration
option(VLGBL_LEGACY "Build legacy bootloader" ON)
option(VLGBL_UEFI "Build UEFI bootloader" ON)

# Output image
set(OUTPUT_IMAGE "${CMAKE_BINARY_DIR}/VolgaOS.iso" CACHE FILEPATH "Path to output image")
