# OpenGL ES 3.0 Renderer for ioq3quest
include_guard(GLOBAL)

if(NOT BUILD_RENDERER_GLES3)
    return()
endif()

message(STATUS "Configuring OpenGL ES 3.0 renderer")

include(renderer_common)

set(RENDERER_GLES3_SOURCES
    ${SOURCE_DIR}/renderergles3/tr_animation.c
    ${SOURCE_DIR}/renderergles3/tr_backend.c
    ${SOURCE_DIR}/renderergles3/tr_bsp.c
    ${SOURCE_DIR}/renderergles3/tr_cmds.c
    ${SOURCE_DIR}/renderergles3/tr_curve.c
    ${SOURCE_DIR}/renderergles3/tr_dsa.c
    ${SOURCE_DIR}/renderergles3/tr_extensions.c
    ${SOURCE_DIR}/renderergles3/tr_extramath.c
    ${SOURCE_DIR}/renderergles3/tr_fbo.c
    ${SOURCE_DIR}/renderergles3/tr_flares.c
    ${SOURCE_DIR}/renderergles3/tr_glsl.c
    ${SOURCE_DIR}/renderergles3/tr_image.c
    ${SOURCE_DIR}/renderergles3/tr_image_dds.c
    ${SOURCE_DIR}/renderergles3/tr_init.c
    ${SOURCE_DIR}/renderergles3/tr_light.c
    ${SOURCE_DIR}/renderergles3/tr_main.c
    ${SOURCE_DIR}/renderergles3/tr_marks.c
    ${SOURCE_DIR}/renderergles3/tr_mesh.c
    ${SOURCE_DIR}/renderergles3/tr_model.c
    ${SOURCE_DIR}/renderergles3/tr_model_iqm.c
    ${SOURCE_DIR}/renderergles3/tr_postprocess.c
    ${SOURCE_DIR}/renderergles3/tr_scene.c
    ${SOURCE_DIR}/renderergles3/tr_shade.c
    ${SOURCE_DIR}/renderergles3/tr_shade_calc.c
    ${SOURCE_DIR}/renderergles3/tr_shader.c
    ${SOURCE_DIR}/renderergles3/tr_shadows.c
    ${SOURCE_DIR}/renderergles3/tr_sky.c
    ${SOURCE_DIR}/renderergles3/tr_surface.c
    ${SOURCE_DIR}/renderergles3/tr_vbo.c
    ${SOURCE_DIR}/renderergles3/tr_world.c
)

# Generate stringified GLSL shaders from .glsl files
set(GLSL_SHADER_DIR ${SOURCE_DIR}/renderergles3/glsl)
set(GENERATED_SHADER_DIR ${CMAKE_BINARY_DIR}/generated/shaders/gles3)
file(MAKE_DIRECTORY ${GENERATED_SHADER_DIR})

# Find all GLSL shader files
file(GLOB GLSL_SHADER_FILES ${GLSL_SHADER_DIR}/*.glsl)

set(RENDERER_GLES3_GLSL_SOURCES)
foreach(GLSL_FILE IN LISTS GLSL_SHADER_FILES)
    get_filename_component(SHADER_NAME ${GLSL_FILE} NAME_WE)
    set(OUTPUT_C_FILE ${GENERATED_SHADER_DIR}/${SHADER_NAME}.c)

    add_custom_command(
        OUTPUT ${OUTPUT_C_FILE}
        COMMAND ${CMAKE_COMMAND}
            -DINPUT_FILE=${GLSL_FILE}
            -DOUTPUT_FILE=${OUTPUT_C_FILE}
            -DSHADER_NAME=${SHADER_NAME}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/utils/stringify_shader.cmake
        DEPENDS ${GLSL_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/utils/stringify_shader.cmake
        COMMENT "Stringifying shader ${SHADER_NAME}"
        VERBATIM
    )

    list(APPEND RENDERER_GLES3_GLSL_SOURCES ${OUTPUT_C_FILE})
endforeach()

# Create a custom target for shader generation
add_custom_target(generate_gles3_shaders DEPENDS ${RENDERER_GLES3_GLSL_SOURCES})

# Combine all GL ES 3 renderer sources
list(APPEND RENDERER_GLES3_BINARY_SOURCES
    ${RENDERER_COMMON_SOURCES}
    ${RENDERER_GLES3_SOURCES}
    ${RENDERER_GLES3_GLSL_SOURCES}
    ${SDL_RENDERER_SOURCES}
    ${RENDERER_LIBRARY_SOURCES}
)

list(APPEND RENDERER_INCLUDE_DIRS
    ${SOURCE_DIR}/renderergles3
)

# Add generated shader directory to include paths
list(APPEND RENDERER_INCLUDE_DIRS
    ${GENERATED_SHADER_DIR}
)
