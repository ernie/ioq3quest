# Android platform configuration
include_guard(GLOBAL)

if(NOT ANDROID)
    return()
endif()

message(STATUS "Configuring for Android platform")

# Android-specific compiler flags
list(APPEND COMMON_DEFINITIONS
    ARCH_STRING="${ARCH_STRING}"
)

# System sources for Android
set(SYSTEM_PLATFORM_SOURCES
    ${SOURCE_DIR}/android/android_snd.c
    ${SOURCE_DIR}/android/sys_android.c
    ${SOURCE_DIR}/sys/sys_unix.c
)

# Console sources
set(CONSOLE_SOURCES
    ${SOURCE_DIR}/sys/con_tty.c
)

# Shared library extension
set(SHLIBEXT "so")

# Client platform-specific sources
set(CLIENT_PLATFORM_SOURCES)

# Android libraries
list(APPEND COMMON_LIBRARIES
    log
    android
)

list(APPEND CLIENT_LIBRARIES
    GLESv3
    OpenSLES
)

list(APPEND RENDERER_LIBRARIES
    GLESv3
    EGL
)

