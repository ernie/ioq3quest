/*
 * vr_gles_renderer.h - GL ES VR rendering
 *
 * Contains the main VR rendering loop for GL ES.
 */

#ifndef __VR_GLES_RENDERER_H
#define __VR_GLES_RENDERER_H

#if __ANDROID__

#include "vr_gles_types.h"

void VR_GetResolution( engine_t* engine, int *pWidth, int *pHeight );
void VR_InitRenderer( engine_t* engine );
void VR_DestroyRenderer( engine_t* engine );
void VR_DrawFrame( engine_t* engine );
void VR_ReInitRenderer(void);

// Loading frame submission for VR - ends current frame and starts new one
// Called during loading states (CA_LOADING/CA_PRIMED) from SCR_UpdateScreen
// so the loading screen is visible in the headset while the game loads
int VR_Renderer_SubmitLoadingFrame( engine_t* engine );

#endif

#endif // __VR_GLES_RENDERER_H
