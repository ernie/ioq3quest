# Common renderer sources shared between GL ES and Vulkan renderers
include_guard(GLOBAL)

set(RENDERER_COMMON_SOURCES
    ${SOURCE_DIR}/renderercommon/tr_font.c
    ${SOURCE_DIR}/renderercommon/tr_image_bmp.c
    ${SOURCE_DIR}/renderercommon/tr_image_jpg.c
    ${SOURCE_DIR}/renderercommon/tr_image_pcx.c
    ${SOURCE_DIR}/renderercommon/tr_image_png.c
    ${SOURCE_DIR}/renderercommon/tr_image_tga.c
    ${SOURCE_DIR}/renderercommon/tr_noise.c
    ${SOURCE_DIR}/qcommon/puff.c
)

set(SDL_RENDERER_SOURCES
    ${SOURCE_DIR}/sdl/sdl_gamma.c
    ${SOURCE_DIR}/sdl/sdl_glimp.c
)

# Renderer include directories
list(APPEND RENDERER_INCLUDE_DIRS
    ${SOURCE_DIR}/renderercommon
)

# Enforce exactly one static renderer
set(_RENDERER_COUNT 0)
if(BUILD_RENDERER_GLES3)
    math(EXPR _RENDERER_COUNT "${_RENDERER_COUNT} + 1")
endif()
if(BUILD_RENDERER_VK)
    math(EXPR _RENDERER_COUNT "${_RENDERER_COUNT} + 1")
endif()

if(_RENDERER_COUNT GREATER 1)
    message(FATAL_ERROR "Multiple static renderers enabled; choose one (BUILD_RENDERER_GLES3 or BUILD_RENDERER_VK)")
elseif(_RENDERER_COUNT EQUAL 0)
    message(FATAL_ERROR "No renderer enabled; choose one (BUILD_RENDERER_GLES3 or BUILD_RENDERER_VK)")
endif()

list(APPEND RENDERER_LIBRARIES ${COMMON_LIBRARIES})
