# OpenAL configuration for Android (dynamic loading)
include_guard(GLOBAL)

option(USE_OPENAL "OpenAL audio" ON)
option(USE_OPENAL_DLOPEN "Dynamically load OpenAL" ON)

if(NOT USE_OPENAL)
    return()
endif()

if(NOT BUILD_CLIENT)
    return()
endif()

message(STATUS "Configuring OpenAL (dynamic loading)")

# OpenAL headers are already in code/AL/
list(APPEND CLIENT_DEFINITIONS USE_OPENAL)

if(USE_OPENAL_DLOPEN)
    list(APPEND CLIENT_DEFINITIONS USE_OPENAL_DLOPEN)
    # Deploy the library for runtime loading
    list(APPEND CLIENT_DEPLOY_LIBRARIES ${SOURCE_DIR}/libs/android/${ANDROID_ABI}/libopenal.so)
else()
    # Static linking - find the prebuilt library
    list(APPEND CLIENT_LIBRARIES ${SOURCE_DIR}/libs/android/${ANDROID_ABI}/libopenal.so)
endif()
