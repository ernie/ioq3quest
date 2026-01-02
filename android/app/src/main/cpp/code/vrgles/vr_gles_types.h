/*
 * vr_gles_types.h - GL ES-specific VR type definitions
 *
 * This header contains VR types that depend on OpenGL ES.
 * Graphics-agnostic types are in vrcommon/vr_types.h
 */

#ifndef __VR_GLES_TYPES_H
#define __VR_GLES_TYPES_H

#include "../vrcommon/vr_types.h"

// GL ES includes
#ifdef USE_LOCAL_HEADERS
#	include "SDL_opengl.h"
#	include "SDL_opengles2.h"
#else
#	include <SDL_opengl.h>
#	include <SDL_opengles2.h>
#endif
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

// OpenXR GL ES extension
#define XR_USE_GRAPHICS_API_OPENGL_ES 1
#include <openxr/openxr_platform.h>

// GL call wrapper macro for debugging
#define GL(func) func;

typedef struct {
    XrSwapchain Handle;
    uint32_t Width;
    uint32_t Height;
} ovrSwapChain;

typedef struct {
    int Width;
    int Height;
    uint32_t TextureSwapChainLength;
    uint32_t TextureSwapChainIndex;
    ovrSwapChain ColorSwapChain;
    XrSwapchainImageOpenGLESKHR* ColorSwapChainImage;
    GLuint* DepthBuffers;
    GLuint* FrameBuffers;
} ovrFramebuffer;

typedef struct {
    ovrFramebuffer FrameBuffer;
} ovrRenderer;

// Full ovrApp struct definition (used by event handlers and elsewhere)
struct ovrApp_s {
    GLboolean Focused;

    XrInstance Instance;
    XrSession Session;
    XrViewConfigurationProperties ViewportConfig;
    XrViewConfigurationView ViewConfigurationView[ovrMaxNumEyes];
    XrSystemId SystemId;
    XrSpace HeadSpace;
    XrSpace LocalFloorSpace;
    XrSpace StageSpace;
    XrSpace FakeStageSpace;
    XrSpace CurrentSpace;
    XrSpace ViewSpace;          // VIEW reference space for head-locked quad layers
    GLboolean SessionActive;

    float* SupportedDisplayRefreshRates;
    uint32_t RequestedDisplayRefreshRateIndex;
    uint32_t NumSupportedDisplayRefreshRates;
    PFN_xrGetDisplayRefreshRateFB pfnGetDisplayRefreshRate;
    PFN_xrRequestDisplayRefreshRateFB pfnRequestDisplayRefreshRate;

    int SwapInterval;
    // These threads will be marked as performance threads.
    int MainThreadTid;
    int RenderThreadTid;
    ovrCompositorLayer_Union Layers[ovrMaxLayerCount];
    int LayerCount;

    ovrRenderer Renderer;
    ovrTrackedController TrackedController[2];

    // Screen overlay swapchain for 2D quad layer (HUD mode 2, vignette, damage, reticle)
    ovrSwapChain OverlaySwapChain;
    XrSwapchainImageOpenGLESKHR* OverlaySwapChainImage;
    uint32_t OverlaySwapChainLength;
    uint32_t OverlaySwapChainIndex;
    GLuint OverlayFrameBuffer;
    GLboolean OverlayAcquired;
};

// Engine state structure
struct engine_s {
	uint64_t frameIndex;
	ovrApp appState;
	ovrJava java;
	float predictedDisplayTime;
};

// Framebuffer operations
void ovrFramebuffer_Clear(ovrFramebuffer* frameBuffer);
bool ovrFramebuffer_Create(
        XrSession session,
        ovrFramebuffer* frameBuffer,
        const int width,
        const int height);
void ovrFramebuffer_Destroy(ovrFramebuffer* frameBuffer);
void ovrFramebuffer_SetCurrent(ovrFramebuffer* frameBuffer);
void ovrFramebuffer_SetNone(void);
void ovrFramebuffer_Resolve(ovrFramebuffer* frameBuffer);
void ovrFramebuffer_Acquire(ovrFramebuffer* frameBuffer);
void ovrFramebuffer_Release(ovrFramebuffer* frameBuffer);

// Renderer operations
void ovrRenderer_Clear(ovrRenderer* renderer);
void ovrRenderer_Create(
        XrSession session,
        ovrRenderer* renderer,
        int suggestedEyeTextureWidth,
        int suggestedEyeTextureHeight);
void ovrRenderer_Destroy(ovrRenderer* renderer);

#endif // __VR_GLES_TYPES_H
