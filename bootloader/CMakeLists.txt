cmake_minimum_required(VERSION 3.20)
project(bootloader)

# Targets
set(MBR_TARGET ${PROJECT_NAME}_mbr)
set(SSL_TARGET ${PROJECT_NAME}_ssl)
set(TSL_TARGET ${PROJECT_NAME}_tsl)

# Docs targets
set(SSL_DOCS ${SSL_TARGET}_docs)
set(TSL_DOCS ${TSL_TARGET}_docs)

# Compile MBR
add_subdirectory(MBR)

# Compile SSL
add_subdirectory(SSL)

# Compile TSL
add_subdirectory(TSL)

add_custom_target(${PROJECT_NAME} 
    DEPENDS ${MBR_TARGET} ${SSL_TARGET} ${TSL_TARGET}
)

if(BUILD_DOCS)
    add_custom_target(${PROJECT_NAME}_docs
    DEPENDS ${SSL_DOCS} ${TSL_DOCS}
    )
endif()

set(MBR_TARGET ${MBR_TARGET} PARENT_SCOPE)
set(SSL_TARGET ${SSL_TARGET} PARENT_SCOPE)
set(TSL_TARGET ${TSL_TARGET} PARENT_SCOPE)
