# Clang compiler configuration for Android
include_guard(GLOBAL)

if(NOT CMAKE_C_COMPILER_ID STREQUAL "Clang")
    return()
endif()

message(STATUS "Configuring Clang compiler settings")

# Base flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -fno-strict-aliasing -Wimplicit -Wstrict-prototypes")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pipe -fno-builtin-cos -fno-builtin-sin -fPIC")

# Optimization flags for release builds
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -funroll-loops -fomit-frame-pointer -ffast-math -DNDEBUG")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -D_DEBUG")

# Shared library flags
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")

# Common compile options (for all targets)
list(APPEND COMMON_COMPILE_OPTIONS
    -Wno-unused-variable
    -Wno-unused-const-variable
    -Wno-format-security
)

# Client-specific compile options
list(APPEND CLIENT_COMPILE_OPTIONS
    -DSDL_DISABLE_IMMINTRIN_H
    -fno-builtin-cos
    -fno-builtin-sin
)

# Game module compile options (cgame, qagame, ui - hide internal symbols)
list(APPEND GAME_MODULE_COMPILE_OPTIONS
    -fvisibility=hidden
)
