# VR sources for ioq3quest
# Phase 4: Full Vulkan VR integration with XR_KHR_vulkan_enable2

include_guard(GLOBAL)

message(STATUS "Configuring VR sources")

# Graphics-agnostic VR sources (vrcommon/)
set(VR_COMMON_SOURCES
    ${SOURCE_DIR}/vrcommon/vr_base.c
    ${SOURCE_DIR}/vrcommon/vr_cvars.c
    ${SOURCE_DIR}/vrcommon/vr_debug.c
    ${SOURCE_DIR}/vrcommon/vr_events.c
    ${SOURCE_DIR}/vrcommon/vr_gameplay.c
    ${SOURCE_DIR}/vrcommon/vr_haptics.c
    ${SOURCE_DIR}/vrcommon/vr_input.c
    ${SOURCE_DIR}/vrcommon/vr_instance.c
    ${SOURCE_DIR}/vrcommon/vr_math.c
    ${SOURCE_DIR}/vrcommon/vr_render_loop.c
    ${SOURCE_DIR}/vrcommon/vr_session.c
    ${SOURCE_DIR}/vrcommon/vr_spaces.c
    ${SOURCE_DIR}/vrcommon/vr_swapchains.c
)

# GL ES specific VR sources (vrgles/)
set(VR_GLES_SOURCES
    ${SOURCE_DIR}/vrgles/vr_gles.c
    ${SOURCE_DIR}/vrgles/vr_gles_framebuffer.c
    ${SOURCE_DIR}/vrgles/vr_gles_events.c
    ${SOURCE_DIR}/vrgles/vr_gles_renderer.c
)

# Vulkan specific VR sources (vrvk/)
# Uses vrcommon/vr_events.c for event handling
set(VR_VK_SOURCES
    ${SOURCE_DIR}/vrvk/vr_vk.c
    ${SOURCE_DIR}/vrvk/vr_vk_debug.c
    ${SOURCE_DIR}/vrvk/vr_vk_session.c
    ${SOURCE_DIR}/vrvk/vr_vk_swapchains.c
    ${SOURCE_DIR}/vrvk/vr_vk_renderer.c
    ${SOURCE_DIR}/vrvk/vr_vk_virtual_screen.c
)

# Renderer-specific VR sources and include directories
if(BUILD_RENDERER_VK)
    set(VR_SOURCES ${VR_COMMON_SOURCES} ${VR_VK_SOURCES})
    list(APPEND CLIENT_INCLUDE_DIRS ${SOURCE_DIR}/vrcommon)
    list(APPEND CLIENT_INCLUDE_DIRS ${SOURCE_DIR}/vrvk)
    list(APPEND RENDERER_INCLUDE_DIRS ${SOURCE_DIR}/vrcommon)
    list(APPEND RENDERER_INCLUDE_DIRS ${SOURCE_DIR}/vrvk)
elseif(BUILD_RENDERER_GLES3)
    set(VR_SOURCES ${VR_COMMON_SOURCES} ${VR_GLES_SOURCES})
    list(APPEND CLIENT_INCLUDE_DIRS ${SOURCE_DIR}/vrcommon)
    list(APPEND CLIENT_INCLUDE_DIRS ${SOURCE_DIR}/vrgles)
    list(APPEND RENDERER_INCLUDE_DIRS ${SOURCE_DIR}/vrcommon)
    list(APPEND RENDERER_INCLUDE_DIRS ${SOURCE_DIR}/vrgles)
else()
    # Default to GL ES sources if no renderer selected
    set(VR_SOURCES ${VR_COMMON_SOURCES} ${VR_GLES_SOURCES})
    list(APPEND CLIENT_INCLUDE_DIRS ${SOURCE_DIR}/vrcommon)
    list(APPEND CLIENT_INCLUDE_DIRS ${SOURCE_DIR}/vrgles)
    list(APPEND RENDERER_INCLUDE_DIRS ${SOURCE_DIR}/vrcommon)
    list(APPEND RENDERER_INCLUDE_DIRS ${SOURCE_DIR}/vrgles)
endif()
