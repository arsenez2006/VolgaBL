# VolgaOS legacy bootloader

## GPT Partition types
* `C586E653-7991-4947-AC24-75F8CFF9945C` - VolgaBL second stage bootloader (SSL)
* `876D0DC7-CF66-4C63-BCEE-BD79EE10F593` - VolgaBL third stage bootloader (TSL)
* `78A9E598-3638-4D67-B2EB-0123D0AFBDBD` - VolgaOS kernel

## Building
### Dependencies
* CMake 3.20+
* NASM
* Doxygen (optional)

### Options
* `-DOUTPUT=<directory>` path to the directory where bootloader images will be placed. Default: `${CMAKE_BINARY_DIR}/out`
* `-DBUILD_DOCS=<boolean>` build docs. Requires Doxygen. Default: `OFF`
* `-DOUTPUT_DOCS=<directory>` path to the directory where docs will be placed. Default: `${OUTPUT}/docs`

### Steps
1. Create build directory
```
mkdir build
```
2. Configure CMake
```
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DOUTPUT=../out
```
3. Run make to build everything
```
cd build
make
```
You can also build only bootloader, without docs
```
cd build
make bootloader
```

### Manual installation
#### Drive mapping
1. Map your drive using GPT (e.g. using `fdisk`)
2. Create 2 partitions for SSL and TSL loaders, each 64KB
3. Assign GPT partition types for each partition
#### Writing images
1. Write `bootloader_mbr` to the first sector of your bootdrive (WARNING: this will erase your current MBR and other OS's won't boot!)
2. Write `bootloader_ssl` to SSL partition
3. Write `bootloader_tsl` to TSL partition
