/*
 * vr_graphics.h - Graphics API abstraction interface
 *
 * This header provides an abstraction layer for graphics-specific VR operations.
 * The actual implementations are in vrvk/vr_vk.c.
 */

#ifndef __VR_GRAPHICS_H
#define __VR_GRAPHICS_H

#include <openxr/openxr.h>

// Get the graphics API extension name for OpenXR instance creation
// Returns XR_KHR_OPENGL_ENABLE_EXTENSION_NAME for OpenGL
// Returns XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME for Vulkan
const char* VR_Graphics_GetExtensionName(void);

// Get graphics requirements from OpenXR runtime
// This must be called after XR instance and system are created
// Returns XR_SUCCESS on success, error code on failure
XrResult VR_Graphics_GetRequirements(XrInstance instance, XrSystemId systemId);

// Print graphics requirements debug info
void VR_Graphics_PrintRequirements(void);

// Initialize graphics-specific VR subsystems
// Called after OpenXR instance is created
// Vulkan: creates VkInstance and VkDevice via xrCreateVulkanInstanceKHR/xrCreateVulkanDeviceKHR
// OpenGL: no-op (context created by SDL later)
void VR_Graphics_Init(XrInstance instance, XrSystemId systemId);

// Shutdown graphics-specific VR subsystems
void VR_Graphics_Shutdown(void);

// Invalidate XR function pointers before XrInstance is destroyed
// This is called from VR_Destroy() before xrDestroyInstance() to ensure
// function pointers obtained via xrGetInstanceProcAddr are cleared.
// Note: This does NOT destroy Vulkan resources - the renderer owns those.
void VR_Graphics_InvalidateFunctionPointers(void);

// Create XR session with graphics-specific binding
// Returns XR_SUCCESS on success
XrResult VR_Graphics_CreateSession(XrInstance instance, XrSystemId systemId, XrSession* session);

#endif // __VR_GRAPHICS_H
