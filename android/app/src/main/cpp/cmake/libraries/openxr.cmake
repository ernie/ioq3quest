# OpenXR configuration for Android
include_guard(GLOBAL)

message(STATUS "Configuring OpenXR")

# OpenXR SDK include directories
set(OPENXR_INCLUDE_DIRS
    ${SOURCE_DIR}/OpenXR/Include
    ${SOURCE_DIR}/OpenXR-SDK/include
)

# OpenXR loader library (Khronos loader from Maven)
set(OPENXR_LIBRARY ${SOURCE_DIR}/OpenXR/Libs/Android/arm64-v8a/libopenxr_loader.so)

# Add to include directories
list(APPEND CLIENT_INCLUDE_DIRS ${OPENXR_INCLUDE_DIRS})
list(APPEND RENDERER_INCLUDE_DIRS ${OPENXR_INCLUDE_DIRS})

# Add library
list(APPEND CLIENT_LIBRARIES ${OPENXR_LIBRARY})
list(APPEND RENDERER_LIBRARIES ${OPENXR_LIBRARY})

# Deploy library to output
list(APPEND CLIENT_DEPLOY_LIBRARIES ${OPENXR_LIBRARY})
