include(ExternalProject)

function(import_VolgaBL)
    cmake_parse_arguments(
        ARG
        ""
        "PATH"
        ""
        ${ARGN}
    )

    ExternalProject_Add(VolgaBL
        SOURCE_DIR ${ARG_PATH}
        BINARY_DIR ${ARG_PATH}/build
        EXCLUDE_FROM_ALL TRUE
        STEP_TARGETS build
        BUILD_ALWAYS TRUE

        CMAKE_CACHE_ARGS
        "-DOUTPUT:PATH=${ARG_PATH}/out"
    )

    set(VOLGABL_TARGET VolgaBL-build PARENT_SCOPE)
    set(VOLGABL_MBR ${ARG_PATH}/out/bootloader_mbr PARENT_SCOPE)
    set(VOLGABL_SSL ${ARG_PATH}/out/bootloader_ssl PARENT_SCOPE)
    set(VOLGABL_TSL ${ARG_PATH}/out/bootloader_tsl PARENT_SCOPE)
endfunction(import_VolgaBL)
