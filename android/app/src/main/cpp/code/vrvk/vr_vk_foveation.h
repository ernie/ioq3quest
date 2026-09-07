/*
 * vr_vk_foveation.h - Foveated rendering profiles (XR_FB_foveation family)
 *
 * Runtime side: the profile on the color swapchain, the density maps it returns, and the
 * gaze. The renderer authors its own map (renderervk/vk.c) and is told strength and centers.
 */

#ifndef __VR_VK_FOVEATION
#define __VR_VK_FOVEATION

#include "../qcommon/q_shared.h"
#include "../vrcommon/vr_types.h"

// The runtime advertises foveation and the device can read a density map
qboolean VR_VK_Foveation_SwapchainWanted(void);

// Call before enumerating images: some runtimes allocate density maps only once a profile is set
qboolean VR_VK_Foveation_ApplyToSwapchain(VR_Engine* engine, XrSwapchain swapchain);

// Live re-apply of vr_foveation. Returns qtrue when density maps appeared and XR resources must be rebuilt
qboolean VR_VK_Foveation_Apply(VR_Engine* engine);

// Profile back to none for a swapchain that got no density maps
void VR_VK_Foveation_Disarm(VR_Engine* engine, XrSwapchain swapchain);

// Per-frame upkeep: re-apply on a cvar or swapchain change, then hand the renderer strength and gaze centers
void VR_VK_Foveation_Frame(VR_Engine* engine);

#endif // __VR_VK_FOVEATION
