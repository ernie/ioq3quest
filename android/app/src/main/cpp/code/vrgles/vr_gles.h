/*
 * vr_gles.h - GL ES VR graphics implementation
 *
 * GL ES-specific declarations and types for VR.
 * Implements the vr_graphics.h interface.
 */

#ifndef __VR_GLES_H
#define __VR_GLES_H

#include "vr_gles_types.h"
#include "../vrcommon/vr_graphics.h"

// GL ES-specific graphics requirements storage
typedef struct {
    XrGraphicsRequirementsOpenGLESKHR requirements;
} VR_GLES_GraphicsRequirements;

// Get the stored graphics requirements (for debugging)
const VR_GLES_GraphicsRequirements* VR_GLES_GetRequirements(void);

#endif // __VR_GLES_H
