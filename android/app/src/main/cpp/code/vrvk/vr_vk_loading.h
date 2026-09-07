/*
 * vr_vk_loading.h - keeps the headset fed while a map load blocks the main thread
 */
#ifndef __VR_VK_LOADING
#define __VR_VK_LOADING

#include "../qcommon/q_shared.h"
#include "../vrcommon/vr_types.h"

// A map load is starting: open the loading window
void VR_Loading_Begin( VR_Engine *engine );
// The main thread is submitting frames of its own again
void VR_Loading_Stop( void );
qboolean VR_Loading_Active( void );
// Main thread only, from inside long loads: submit a frame if it has been a while since the last
void VR_Loading_Pump( VR_Engine *engine );
// The main thread just submitted a frame; the pump paces itself from this
void VR_Loading_NoteMainFrame( void );
void VR_Loading_SetColorReleased( qboolean released );
// Before the session goes
void VR_Loading_Shutdown( void );

#endif
