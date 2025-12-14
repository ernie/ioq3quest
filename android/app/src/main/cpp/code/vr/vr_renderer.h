#ifndef __VR_RENDERER
#define __VR_RENDERER

#if __ANDROID__

#include "vr_types.h"

void VR_GetResolution( engine_t* engine, int *pWidth, int *pHeight );
void VR_InitRenderer( engine_t* engine );
void VR_DestroyRenderer( engine_t* engine );
void VR_DrawFrame( engine_t* engine );
void VR_ReInitRenderer();

// Two-phase loading frame submission for VR:
// 1. VR_PrepareLoadingFrame - call BEFORE rendering to set up VR framebuffer
// 2. VR_SubmitLoadingFrame - call AFTER rendering to submit to headset
int VR_PrepareLoadingFrame( engine_t* engine );
int VR_SubmitLoadingFrame( engine_t* engine );

#endif

#endif
