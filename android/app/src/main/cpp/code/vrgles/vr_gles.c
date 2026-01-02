/*
 * vr_gles.c - GL ES VR graphics implementation
 *
 * Implements the vr_graphics.h interface for OpenGL ES.
 */

#include "vr_gles.h"
#include <string.h>

// Static storage for graphics requirements
static VR_GLES_GraphicsRequirements s_graphicsRequirements;
static PFN_xrGetOpenGLESGraphicsRequirementsKHR s_pfnGetOpenGLESGraphicsRequirementsKHR = NULL;

// Virtual screen state
static float s_virtualScreenYaw = 0.0f;

const char* VR_Graphics_GetExtensionName(void) {
    return XR_KHR_OPENGL_ES_ENABLE_EXTENSION_NAME;
}

XrResult VR_Graphics_GetRequirements(XrInstance instance, XrSystemId systemId) {
    // Get the function pointer if we don't have it yet
    if (s_pfnGetOpenGLESGraphicsRequirementsKHR == NULL) {
        XrResult result = xrGetInstanceProcAddr(
                instance,
                "xrGetOpenGLESGraphicsRequirementsKHR",
                (PFN_xrVoidFunction*)(&s_pfnGetOpenGLESGraphicsRequirementsKHR));
        if (result != XR_SUCCESS || s_pfnGetOpenGLESGraphicsRequirementsKHR == NULL) {
            ALOGE("Failed to get xrGetOpenGLESGraphicsRequirementsKHR function pointer");
            return result;
        }
    }

    // Initialize the requirements struct
    memset(&s_graphicsRequirements.requirements, 0, sizeof(s_graphicsRequirements.requirements));
    s_graphicsRequirements.requirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_ES_KHR;

    // Get the graphics requirements
    XrResult result = s_pfnGetOpenGLESGraphicsRequirementsKHR(
            instance, systemId, &s_graphicsRequirements.requirements);

    return result;
}

void VR_Graphics_PrintRequirements(void) {
    ALOGV("OpenGL ES Graphics Requirements:");
    ALOGV("  Min API Version: %d.%d.%d",
            XR_VERSION_MAJOR(s_graphicsRequirements.requirements.minApiVersionSupported),
            XR_VERSION_MINOR(s_graphicsRequirements.requirements.minApiVersionSupported),
            XR_VERSION_PATCH(s_graphicsRequirements.requirements.minApiVersionSupported));
    ALOGV("  Max API Version: %d.%d.%d",
            XR_VERSION_MAJOR(s_graphicsRequirements.requirements.maxApiVersionSupported),
            XR_VERSION_MINOR(s_graphicsRequirements.requirements.maxApiVersionSupported),
            XR_VERSION_PATCH(s_graphicsRequirements.requirements.maxApiVersionSupported));
}

void VR_Graphics_Init(XrInstance instance, XrSystemId systemId) {
    // For GL ES, the EGL context is created elsewhere (by SDL/Android)
    // This function is a no-op for GL ES
    ALOGV("VR_Graphics_Init: GL ES - no initialization needed (EGL context created externally)");
}

void VR_Graphics_Shutdown(void) {
    // For GL ES, EGL context is managed externally
    ALOGV("VR_Graphics_Shutdown: GL ES - no shutdown needed");
}

void VR_Graphics_InvalidateFunctionPointers(void) {
    // Clear the function pointer so it will be re-acquired if needed
    s_pfnGetOpenGLESGraphicsRequirementsKHR = NULL;
}

XrResult VR_Graphics_CreateSession(XrInstance instance, XrSystemId systemId, XrSession* session) {
    // Create the graphics binding for GL ES on Android
    XrGraphicsBindingOpenGLESAndroidKHR graphicsBinding = {};
    graphicsBinding.type = XR_TYPE_GRAPHICS_BINDING_OPENGL_ES_ANDROID_KHR;
    graphicsBinding.next = NULL;
    graphicsBinding.display = eglGetCurrentDisplay();
    graphicsBinding.config = NULL;
    graphicsBinding.context = eglGetCurrentContext();

    if (graphicsBinding.display == EGL_NO_DISPLAY) {
        ALOGE("VR_Graphics_CreateSession: No EGL display available");
        return XR_ERROR_INITIALIZATION_FAILED;
    }

    if (graphicsBinding.context == EGL_NO_CONTEXT) {
        ALOGE("VR_Graphics_CreateSession: No EGL context available");
        return XR_ERROR_INITIALIZATION_FAILED;
    }

    // Create the session
    XrSessionCreateInfo sessionCreateInfo = {};
    sessionCreateInfo.type = XR_TYPE_SESSION_CREATE_INFO;
    sessionCreateInfo.next = &graphicsBinding;
    sessionCreateInfo.createFlags = 0;
    sessionCreateInfo.systemId = systemId;

    XrResult result = xrCreateSession(instance, &sessionCreateInfo, session);

    if (result == XR_SUCCESS) {
        ALOGV("VR_Graphics_CreateSession: XR session created successfully");
    } else {
        ALOGE("VR_Graphics_CreateSession: Failed to create XR session: %d", result);
    }

    return result;
}

void VR_VirtualScreen_ResetPosition(void) {
    s_virtualScreenYaw = 0.0f;
}

float VR_VirtualScreen_GetCurrentYaw(void) {
    return s_virtualScreenYaw;
}

int VR_GetVirtualScreenMVP(float screenMVP[2][16], float floorMVP[2][16]) {
    // GL ES uses a different virtual screen approach (cylinder layer)
    // This MVP-based approach is only used by Vulkan renderer
    (void)screenMVP;
    (void)floorMVP;
    return 0;  // qfalse
}

const VR_GLES_GraphicsRequirements* VR_GLES_GetRequirements(void) {
    return &s_graphicsRequirements;
}
