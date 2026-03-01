# Internal zlib configuration
include_guard(GLOBAL)

option(USE_INTERNAL_ZLIB "Use internal copy of zlib" ON)

if(NOT USE_INTERNAL_ZLIB)
    return()
endif()

message(STATUS "Using internal zlib")

set(ZLIB_DIR ${SOURCE_DIR}/zlib)

set(ZLIB_SOURCES
    ${ZLIB_DIR}/adler32.c
    ${ZLIB_DIR}/crc32.c
    ${ZLIB_DIR}/inffast.c
    ${ZLIB_DIR}/inflate.c
    ${ZLIB_DIR}/inftrees.c
    ${ZLIB_DIR}/zutil.c
)

list(APPEND CLIENT_INCLUDE_DIRS ${ZLIB_DIR})
list(APPEND CLIENT_LIBRARY_SOURCES ${ZLIB_SOURCES})

# Suppress warnings in vendored zlib code
set_source_files_properties(${ZLIB_SOURCES} PROPERTIES COMPILE_FLAGS -w)
