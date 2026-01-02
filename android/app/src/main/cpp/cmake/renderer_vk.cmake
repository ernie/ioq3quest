include_guard(GLOBAL)

if(NOT BUILD_RENDERER_VK)
    return()
endif()

message(STATUS "Configuring Vulkan renderer")

include(renderer_common)
include(compile_shaders)

set(RENDERER_VK_SOURCES
    ${SOURCE_DIR}/renderervk/tr_animation.c
    ${SOURCE_DIR}/renderervk/tr_backend.c
    ${SOURCE_DIR}/renderervk/tr_bsp.c
    ${SOURCE_DIR}/renderervk/tr_cmds.c
    ${SOURCE_DIR}/renderervk/tr_curve.c
    ${SOURCE_DIR}/renderervk/tr_image.c
    ${SOURCE_DIR}/renderervk/tr_init.c
    ${SOURCE_DIR}/renderervk/tr_light.c
    ${SOURCE_DIR}/renderervk/tr_main.c
    ${SOURCE_DIR}/renderervk/tr_marks.c
    ${SOURCE_DIR}/renderervk/tr_mesh.c
    ${SOURCE_DIR}/renderervk/tr_model.c
    ${SOURCE_DIR}/renderervk/tr_model_iqm.c
    ${SOURCE_DIR}/renderervk/tr_scene.c
    ${SOURCE_DIR}/renderervk/tr_shade.c
    ${SOURCE_DIR}/renderervk/tr_shade_calc.c
    ${SOURCE_DIR}/renderervk/tr_shader.c
    ${SOURCE_DIR}/renderervk/tr_shadows.c
    ${SOURCE_DIR}/renderervk/tr_sky.c
    ${SOURCE_DIR}/renderervk/tr_surface.c
    ${SOURCE_DIR}/renderervk/tr_world.c
    ${SOURCE_DIR}/renderervk/vk.c
    ${SOURCE_DIR}/renderervk/vk_flares.c
    ${SOURCE_DIR}/renderervk/vk_vbo.c
)

# Combine all Vulkan renderer sources
list(APPEND RENDERER_VK_BINARY_SOURCES
    ${RENDERER_COMMON_SOURCES}
    ${RENDERER_VK_SOURCES}
    ${SDL_RENDERER_SOURCES}
    ${RENDERER_LIBRARY_SOURCES}
)

# Find Vulkan via NDK
find_library(VULKAN_LIB vulkan)
list(APPEND RENDERER_LIBRARIES ${VULKAN_LIB})

# Add renderervk to include directories
list(APPEND RENDERER_INCLUDE_DIRS
    ${SOURCE_DIR}/renderervk
)
