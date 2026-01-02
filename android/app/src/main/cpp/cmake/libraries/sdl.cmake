# SDL2 configuration for Android
include_guard(GLOBAL)

message(STATUS "Configuring SDL2")

# SDL2 include directories
set(SDL_INCLUDE_DIRS
    ${SOURCE_DIR}/SDL2/include
)

# SDL2 library (pre-built for Android arm64)
set(SDL_LIBRARY ${SOURCE_DIR}/libs/android/arm64-v8a/libSDL2.so)

# Add to include directories
list(APPEND CLIENT_INCLUDE_DIRS ${SDL_INCLUDE_DIRS})
list(APPEND RENDERER_INCLUDE_DIRS ${SDL_INCLUDE_DIRS})

# Add library
list(APPEND CLIENT_LIBRARIES ${SDL_LIBRARY})
list(APPEND RENDERER_LIBRARIES ${SDL_LIBRARY})

# Deploy library to output
list(APPEND CLIENT_DEPLOY_LIBRARIES ${SDL_LIBRARY})
