# Internal libvorbis configuration
include_guard(GLOBAL)

option(USE_CODEC_VORBIS "Ogg Vorbis support" ON)
option(USE_INTERNAL_VORBIS "Use internal copy of libvorbis" ON)

if(NOT USE_CODEC_VORBIS)
    return()
endif()

list(APPEND CLIENT_DEFINITIONS USE_CODEC_VORBIS)

if(NOT USE_INTERNAL_VORBIS)
    return()
endif()

message(STATUS "Using internal libvorbis-1.3.6")

set(VORBIS_DIR ${SOURCE_DIR}/libvorbis-1.3.6)
set(OGG_DIR ${SOURCE_DIR}/libogg-1.3.3)

set(VORBIS_SOURCES
    ${VORBIS_DIR}/lib/analysis.c
    ${VORBIS_DIR}/lib/bitrate.c
    ${VORBIS_DIR}/lib/block.c
    ${VORBIS_DIR}/lib/codebook.c
    ${VORBIS_DIR}/lib/envelope.c
    ${VORBIS_DIR}/lib/floor0.c
    ${VORBIS_DIR}/lib/floor1.c
    ${VORBIS_DIR}/lib/info.c
    ${VORBIS_DIR}/lib/lookup.c
    ${VORBIS_DIR}/lib/lpc.c
    ${VORBIS_DIR}/lib/lsp.c
    ${VORBIS_DIR}/lib/mapping0.c
    ${VORBIS_DIR}/lib/mdct.c
    ${VORBIS_DIR}/lib/psy.c
    ${VORBIS_DIR}/lib/registry.c
    ${VORBIS_DIR}/lib/res0.c
    ${VORBIS_DIR}/lib/smallft.c
    ${VORBIS_DIR}/lib/sharedbook.c
    ${VORBIS_DIR}/lib/synthesis.c
    ${VORBIS_DIR}/lib/vorbisfile.c
    ${VORBIS_DIR}/lib/window.c
)

# Create object library with vorbis-specific includes to avoid mdct.h conflict with opus
# Both vorbis and opus define mdct_lookup with different structures
add_library(vorbis_objects OBJECT ${VORBIS_SOURCES})
target_include_directories(vorbis_objects PRIVATE
    ${VORBIS_DIR}/include
    ${VORBIS_DIR}/lib
    ${OGG_DIR}/include
)
target_compile_options(vorbis_objects PRIVATE ${COMMON_COMPILE_OPTIONS} -w)

# Add the vorbis include directory for the main client (for vorbisfile.h etc)
# Do NOT add lib/ to global includes as it contains mdct.h which conflicts with opus
list(APPEND CLIENT_INCLUDE_DIRS ${VORBIS_DIR}/include)

# Export the object library to link with client
list(APPEND CLIENT_OBJECT_LIBRARIES vorbis_objects)
