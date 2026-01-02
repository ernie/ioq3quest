# Internal libogg configuration
include_guard(GLOBAL)

option(USE_INTERNAL_OGG "Use internal copy of libogg" ON)

if(NOT USE_INTERNAL_OGG)
    return()
endif()

message(STATUS "Using internal libogg-1.3.3")

set(OGG_DIR ${SOURCE_DIR}/libogg-1.3.3)

set(OGG_SOURCES
    ${OGG_DIR}/src/bitwise.c
    ${OGG_DIR}/src/framing.c
)

list(APPEND CLIENT_INCLUDE_DIRS ${OGG_DIR}/include)
list(APPEND CLIENT_LIBRARY_SOURCES ${OGG_SOURCES})
