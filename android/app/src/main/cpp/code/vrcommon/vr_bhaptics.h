/*
 * vr_bhaptics.h - bHaptics stub for Quest
 *
 * bHaptics support is for PCVR only (uses hardware vest/arm haptics).
 * This stub provides no-op implementations for Quest builds.
 */

#ifndef __VR_BHAPTICS
#define __VR_BHAPTICS

#include "../qcommon/q_shared.h"

// Stub implementations: bHaptics is PCVR-only
static inline void VR_Bhaptics_Init(void) {}
static inline void VR_Bhaptics_Shutdown(void) {}
static inline void VR_Bhaptics_UpdateEnabled(void) {}
static inline void VR_Bhaptics_HandleEvent(const char* event, int position, int intensity, float yaw, float height) {
	(void)event; (void)position; (void)intensity; (void)yaw; (void)height;
}

#endif // __VR_BHAPTICS
