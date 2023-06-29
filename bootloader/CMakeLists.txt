cmake_minimum_required(VERSION 3.20)
project(VLGBL)

if(VLGBL_LEGACY)
    add_subdirectory(legacy)
endif()

add_custom_target(${PROJECT_NAME} 
    DEPENDS ${BOOTLOADER_LEGACY_DEPENDENCY}
)

if(BUILD_DOCS)
    add_custom_target(${PROJECT_NAME}_docs
        DEPENDS ${BOOTLOADER_LEGACY_DOCS_DEPENDENCY}
    )
endif()
