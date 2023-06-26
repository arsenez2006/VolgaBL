function(create_image)
    # Create disk_layout.map
    set(IMAGE_LAYOUT ${CMAKE_BINARY_DIR}/image_layout.map)
    if(NOT EXISTS ${IMAGE_LAYOUT})
        file(TOUCH ${IMAGE_LAYOUT})
    else()
        file(WRITE ${IMAGE_LAYOUT} "")
    endif()
    
    file(APPEND ${IMAGE_LAYOUT} "label: gpt\n")
    file(APPEND ${IMAGE_LAYOUT} "unit: sectors\n")
    file(APPEND ${IMAGE_LAYOUT} "sector-size: 512\n")

    # Add kernel Partition
    file(APPEND ${IMAGE_LAYOUT} "kernel: size=16M, type=78A9E598-3638-4D67-B2EB-0123D0AFBDBD\n")

    # Add bootloader
    if(VLGBL_LEGACY)
        file(APPEND ${IMAGE_LAYOUT} "SSL: size=64K, type=C586E653-7991-4947-AC24-75F8CFF9945C\n")
        file(APPEND ${IMAGE_LAYOUT} "TSL: size=64K, type=876D0DC7-CF66-4C63-BCEE-BD79EE10F593\n")
    endif()

    # Create image
    if(NOT EXISTS ${OUTPUT_IMAGE})
        exec_program(${DD}
            ARGS if=/dev/zero of=${OUTPUT_IMAGE} bs=$[1024 * 1024] count=$[128]
            # OUTPUT_VARIABLE SILENT
        )
    endif()

    # Map image
    exec_program(${SFDISK}
        ARGS ${OUTPUT_IMAGE} < ${IMAGE_LAYOUT}
    )

    # Get the actual image map
    set(MAP_FILE ${CMAKE_BINARY_DIR}/map.map)
    exec_program(${SFDISK} 
        ARGS -d ${OUTPUT_IMAGE}
        OUTPUT_VARIABLE MAP
    )
    # Remove whitespaces and tabs
    string(REGEX REPLACE "[ \t]" "" MAP ${MAP})
    # Save into file
    file(WRITE ${MAP_FILE} ${MAP})

    # Get bootloader variables
    if(VLGBL_LEGACY)
        # Get map entries
        file(STRINGS ${MAP_FILE} SSL_MAP_ENTRY REGEX "type=C586E653-7991-4947-AC24-75F8CFF9945C")
        file(STRINGS ${MAP_FILE} TSL_MAP_ENTRY REGEX "type=876D0DC7-CF66-4C63-BCEE-BD79EE10F593")

        # For MBR
        set(OUTPUT_MBR_BS 1)
        set(OUTPUT_MBR_OFFSET 0)
        set(OUTPUT_MBR_SIZE 446)

        # For SSL
        set(OUTPUT_SSL_BS 512)

        string(REGEX MATCH "start=[0-9]+" SSL_START_ENTRY ${SSL_MAP_ENTRY})
        string(REGEX MATCH "size=[0-9]+" SSL_SIZE_ENTRY ${SSL_MAP_ENTRY})

        string(REGEX MATCH "[0-9]+" OUTPUT_SSL_OFFSET ${SSL_START_ENTRY})
        string(REGEX MATCH "[0-9]+" OUTPUT_SSL_SIZE ${SSL_SIZE_ENTRY})

        # For TSL
        set(OUTPUT_TSL_BS 512)

        string(REGEX MATCH "start=[0-9]+" TSL_START_ENTRY ${TSL_MAP_ENTRY})
        string(REGEX MATCH "size=[0-9]+" TSL_SIZE_ENTRY ${TSL_MAP_ENTRY})

        string(REGEX MATCH "[0-9]+" OUTPUT_TSL_OFFSET ${TSL_START_ENTRY})
        string(REGEX MATCH "[0-9]+" OUTPUT_TSL_SIZE ${TSL_SIZE_ENTRY})
    endif()
endfunction(create_image)
