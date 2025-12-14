#include "vr_base.h"
#include "vr_renderer.h"

#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"
#include "../client/client.h"

#include "vr_clientinfo.h"
#include "vr_input.h"
#include "vr_types.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ENABLE_GL_DEBUG 0
#define ENABLE_GL_DEBUG_VERBOSE 0
#if ENABLE_GL_DEBUG
#include <GLES3/gl32.h>
#endif

#define DEFAULT_SUPER_SAMPLING  1.1f

extern vr_clientinfo_t vr;
extern cvar_t *vr_heightAdjust;
extern cvar_t *vr_screenCurvature;

XrView* projections;
GLboolean stageSupported = GL_FALSE;
GLboolean localFloorSupported = GL_FALSE;
qboolean fullscreenMode = qfalse;
qboolean needRecenter = qtrue;

void VR_UpdateStageBounds(ovrApp* pappState) {
    XrExtent2Df stageBounds = {};

    // Try LOCAL_FLOOR bounds first (preferred space on Meta Quest)
    XrResult result;
    OXR(result = xrGetReferenceSpaceBoundsRect(
            pappState->Session, XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR, &stageBounds));
    if (result != XR_SUCCESS) {
        ALOGV("LOCAL_FLOOR bounds query failed: using small defaults and FakeStageSpace");
        stageBounds.width = 1.0f;
        stageBounds.height = 1.0f;
        pappState->CurrentSpace = pappState->FakeStageSpace;
    }

    ALOGV("Stage bounds: width = %f, depth %f", stageBounds.width, stageBounds.height);
}


void APIENTRY VR_GLDebugLog(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_PERFORMANCE || ENABLE_GL_DEBUG_VERBOSE)
	{
		char typeStr[128];
		switch (type) {
			case GL_DEBUG_TYPE_ERROR: sprintf(typeStr, "ERROR"); break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: sprintf(typeStr, "DEPRECATED_BEHAVIOR"); break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: sprintf(typeStr, "UNDEFINED_BEHAVIOR"); break;
			case GL_DEBUG_TYPE_PORTABILITY: sprintf(typeStr, "PORTABILITY"); break;
			case GL_DEBUG_TYPE_PERFORMANCE: sprintf(typeStr, "PERFORMANCE"); break;
			case GL_DEBUG_TYPE_MARKER: sprintf(typeStr, "MARKER"); break;
			case GL_DEBUG_TYPE_PUSH_GROUP: sprintf(typeStr, "PUSH_GROUP"); break;
			case GL_DEBUG_TYPE_POP_GROUP: sprintf(typeStr, "POP_GROUP"); break;
			default: sprintf(typeStr, "OTHER"); break;
		}

		char severinityStr[128];
		switch (severity) {
			case GL_DEBUG_SEVERITY_HIGH: sprintf(severinityStr, "HIGH"); break;
			case GL_DEBUG_SEVERITY_MEDIUM: sprintf(severinityStr, "MEDIUM"); break;
			case GL_DEBUG_SEVERITY_LOW: sprintf(severinityStr, "LOW"); break;
			default: sprintf(severinityStr, "VERBOSE"); break;
		}

		Com_Printf("[%s] GL issue - %s: %s\n", severinityStr, typeStr, message);
	}
}

void VR_GetResolution(engine_t* engine, int *pWidth, int *pHeight)
{
	static int width = 0;
	static int height = 0;
	float superSampling = 0.0f;

	float configuredSuperSampling = Cvar_VariableValue("vr_superSampling");
	if (vr.superSampling == 0.0f || configuredSuperSampling != vr.superSampling) {
		vr.superSampling = configuredSuperSampling;
		if (vr.superSampling != 0.0f) {
			Cbuf_AddText( "vid_restart\n" );
		}
	}

	if (vr.superSampling == 0.0f) {
		superSampling = DEFAULT_SUPER_SAMPLING;
	} else {
		superSampling = vr.superSampling;
	}

	if (engine)
	{
        // Enumerate the viewport configurations.
        uint32_t viewportConfigTypeCount = 0;
        OXR(xrEnumerateViewConfigurations(
                engine->appState.Instance, engine->appState.SystemId, 0, &viewportConfigTypeCount, NULL));

        XrViewConfigurationType* viewportConfigurationTypes =
                (XrViewConfigurationType*)malloc(viewportConfigTypeCount * sizeof(XrViewConfigurationType));

        OXR(xrEnumerateViewConfigurations(
                engine->appState.Instance,
                engine->appState.SystemId,
                viewportConfigTypeCount,
                &viewportConfigTypeCount,
                viewportConfigurationTypes));

        ALOGV("Available Viewport Configuration Types: %d", viewportConfigTypeCount);

        for (uint32_t i = 0; i < viewportConfigTypeCount; i++) {
            const XrViewConfigurationType viewportConfigType = viewportConfigurationTypes[i];

            ALOGV(
                    "Viewport configuration type %d : %s",
                    viewportConfigType,
                    viewportConfigType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO ? "Selected" : "");

            XrViewConfigurationProperties viewportConfig;
            viewportConfig.type = XR_TYPE_VIEW_CONFIGURATION_PROPERTIES;
            OXR(xrGetViewConfigurationProperties(
                    engine->appState.Instance, engine->appState.SystemId, viewportConfigType, &viewportConfig));
            ALOGV(
                    "FovMutable=%s ConfigurationType %d",
                    viewportConfig.fovMutable ? "true" : "false",
                    viewportConfig.viewConfigurationType);

            uint32_t viewCount;
            OXR(xrEnumerateViewConfigurationViews(
                    engine->appState.Instance, engine->appState.SystemId, viewportConfigType, 0, &viewCount, NULL));

            if (viewCount > 0) {
                XrViewConfigurationView* elements =
                        (XrViewConfigurationView*)malloc(viewCount * sizeof(XrViewConfigurationView));

                for (uint32_t e = 0; e < viewCount; e++) {
                    elements[e].type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
                    elements[e].next = NULL;
                }

                OXR(xrEnumerateViewConfigurationViews(
                        engine->appState.Instance,
                        engine->appState.SystemId,
                        viewportConfigType,
                        viewCount,
                        &viewCount,
                        elements));

                // Cache the view config properties for the selected config type.
                if (viewportConfigType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO) {
                    assert(viewCount == ovrMaxNumEyes);
                    for (uint32_t e = 0; e < viewCount; e++) {
                        engine->appState.ViewConfigurationView[e] = elements[e];
                    }
                }

                free(elements);
            } else {
                ALOGE("Empty viewport configuration type: %d", viewCount);
            }
        }

        free(viewportConfigurationTypes);

        *pWidth = width = engine->appState.ViewConfigurationView[0].recommendedImageRectWidth * superSampling;
        *pHeight = height = engine->appState.ViewConfigurationView[0].recommendedImageRectHeight * superSampling;
	}
	else
	{
		//use cached values
		*pWidth = width;
		*pHeight = height;
	}
}

void VR_Recenter(engine_t* engine) {
    // Calculate recenter reference
    XrReferenceSpaceCreateInfo spaceCreateInfo = {};
    spaceCreateInfo.type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO;
    spaceCreateInfo.poseInReferenceSpace.orientation.w = 1.0f;

    // Only try to get current head position if we have a valid time
    // (predictedDisplayTime is only set after xrWaitFrame returns)
    if (engine->appState.CurrentSpace != XR_NULL_HANDLE && engine->predictedDisplayTime != 0) {
        vec3_t rotation = {0, 0, 0};
        XrSpaceLocation loc = {};
        loc.type = XR_TYPE_SPACE_LOCATION;
        OXR(xrLocateSpace(engine->appState.HeadSpace, engine->appState.CurrentSpace, engine->predictedDisplayTime, &loc));
        QuatToYawPitchRoll(loc.pose.orientation, rotation, vr.hmdorientation);

        vr.recenterYaw += radians(vr.hmdorientation[YAW]);
        spaceCreateInfo.poseInReferenceSpace.orientation.x = 0;
        spaceCreateInfo.poseInReferenceSpace.orientation.y = sin(vr.recenterYaw / 2);
        spaceCreateInfo.poseInReferenceSpace.orientation.z = 0;
        spaceCreateInfo.poseInReferenceSpace.orientation.w = cos(vr.recenterYaw / 2);
    }

    // Delete previous space instances
    if (engine->appState.LocalFloorSpace != XR_NULL_HANDLE) {
        OXR(xrDestroySpace(engine->appState.LocalFloorSpace));
        engine->appState.LocalFloorSpace = XR_NULL_HANDLE;
    }
    if (engine->appState.StageSpace != XR_NULL_HANDLE) {
        OXR(xrDestroySpace(engine->appState.StageSpace));
        engine->appState.StageSpace = XR_NULL_HANDLE;
    }
    if (engine->appState.FakeStageSpace != XR_NULL_HANDLE) {
        OXR(xrDestroySpace(engine->appState.FakeStageSpace));
        engine->appState.FakeStageSpace = XR_NULL_HANDLE;
    }

    // Create reference spaces in order of preference:
    // 1. LOCAL_FLOOR - Preferred, recenterable floor-relative space (like VrApi's LOCAL_FLOOR)
    // 2. LOCAL with offset - Fallback for systems that don't support LOCAL_FLOOR
    // Note: STAGE is NOT used as CurrentSpace on Meta Quest because it's anchored to play area
    // center rather than user position, causing offset issues with stationary boundary.

    // Always create a fallback space using LOCAL with floor offset
    spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    spaceCreateInfo.poseInReferenceSpace.position.y = -1.6750f;  // Approximate floor offset for Meta Quest
    OXR(xrCreateReferenceSpace(engine->appState.Session, &spaceCreateInfo, &engine->appState.FakeStageSpace));
    engine->appState.CurrentSpace = engine->appState.FakeStageSpace;
    ALOGV("VR_Recenter: Created FakeStageSpace (LOCAL with floor offset) as fallback");

    // Create STAGE space for bounds queries only (not used as CurrentSpace)
    if (stageSupported) {
        spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
        spaceCreateInfo.poseInReferenceSpace.position.y = 0.0f;
        OXR(xrCreateReferenceSpace(engine->appState.Session, &spaceCreateInfo, &engine->appState.StageSpace));
        ALOGV("VR_Recenter: Created StageSpace (STAGE) for bounds queries only");
    }

    // Prefer LOCAL_FLOOR if supported - this is the closest equivalent to VrApi's LOCAL_FLOOR
    // It provides floor-relative vertical position with recenterable horizontal position
    if (localFloorSupported) {
        spaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR;
        spaceCreateInfo.poseInReferenceSpace.position.y = 0.0f;
        OXR(xrCreateReferenceSpace(engine->appState.Session, &spaceCreateInfo, &engine->appState.LocalFloorSpace));
        engine->appState.CurrentSpace = engine->appState.LocalFloorSpace;
        ALOGV("VR_Recenter: Created LocalFloorSpace (LOCAL_FLOOR) - PREFERRED");
    }

    ALOGV("VR_Recenter: CurrentSpace set to %s",
          engine->appState.CurrentSpace == engine->appState.LocalFloorSpace ? "LOCAL_FLOOR" :
          "FAKE_STAGE (LOCAL)");

    // Update menu orientation
    vr.menuYaw = 0;
}

void VR_InitRenderer( engine_t* engine ) {
	ALOGV("VR_InitRenderer: starting");

	// Check if session exists - if not, skip initialization
	// (will be called again after VR_EnterVR creates the session)
	if (engine->appState.Session == XR_NULL_HANDLE) {
		ALOGV("VR_InitRenderer: no session yet, skipping");
		return;
	}

#if ENABLE_GL_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(VR_GLDebugLog, 0);
#endif

	int eyeW, eyeH;
    VR_GetResolution(engine, &eyeW, &eyeH);
    ALOGV("VR_InitRenderer: resolution %dx%d", eyeW, eyeH);

    // Get the viewport configuration info for the chosen viewport configuration type.
    engine->appState.ViewportConfig.type = XR_TYPE_VIEW_CONFIGURATION_PROPERTIES;

    OXR(xrGetViewConfigurationProperties(
            engine->appState.Instance, engine->appState.SystemId, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, &engine->appState.ViewportConfig));
    ALOGV("VR_InitRenderer: ViewportConfig.viewConfigurationType=%d", engine->appState.ViewportConfig.viewConfigurationType);

    // Get the supported display refresh rates for the system.
    {
        PFN_xrEnumerateDisplayRefreshRatesFB pfnxrEnumerateDisplayRefreshRatesFB = NULL;
        OXR(xrGetInstanceProcAddr(
                engine->appState.Instance,
                "xrEnumerateDisplayRefreshRatesFB",
                (PFN_xrVoidFunction*)(&pfnxrEnumerateDisplayRefreshRatesFB)));

        OXR(pfnxrEnumerateDisplayRefreshRatesFB(
                engine->appState.Session, 0, &engine->appState.NumSupportedDisplayRefreshRates, NULL));

        engine->appState.SupportedDisplayRefreshRates =
                (float*)malloc(engine->appState.NumSupportedDisplayRefreshRates * sizeof(float));
        OXR(pfnxrEnumerateDisplayRefreshRatesFB(
                engine->appState.Session,
                engine->appState.NumSupportedDisplayRefreshRates,
                &engine->appState.NumSupportedDisplayRefreshRates,
                engine->appState.SupportedDisplayRefreshRates));
        ALOGV("Supported Refresh Rates:");
        for (uint32_t i = 0; i < engine->appState.NumSupportedDisplayRefreshRates; i++) {
            ALOGV("%d:%f", i, engine->appState.SupportedDisplayRefreshRates[i]);
        }

        OXR(xrGetInstanceProcAddr(
                engine->appState.Instance,
                "xrGetDisplayRefreshRateFB",
                (PFN_xrVoidFunction*)(&engine->appState.pfnGetDisplayRefreshRate)));

        float currentDisplayRefreshRate = 0.0f;
        OXR(engine->appState.pfnGetDisplayRefreshRate(engine->appState.Session, &currentDisplayRefreshRate));
        ALOGV("Current System Display Refresh Rate: %f", currentDisplayRefreshRate);

        OXR(xrGetInstanceProcAddr(
                engine->appState.Instance,
                "xrRequestDisplayRefreshRateFB",
                (PFN_xrVoidFunction*)(&engine->appState.pfnRequestDisplayRefreshRate)));

        // Test requesting the system default.
        OXR(engine->appState.pfnRequestDisplayRefreshRate(engine->appState.Session, 0.0f));
        ALOGV("Requesting system default display refresh rate");
    }

    uint32_t numOutputSpaces = 0;
    OXR(xrEnumerateReferenceSpaces(engine->appState.Session, 0, &numOutputSpaces, NULL));

    XrReferenceSpaceType* referenceSpaces =
            (XrReferenceSpaceType*)malloc(numOutputSpaces * sizeof(XrReferenceSpaceType));

    OXR(xrEnumerateReferenceSpaces(
            engine->appState.Session, numOutputSpaces, &numOutputSpaces, referenceSpaces));

    ALOGV("Supported reference spaces (%d total):", numOutputSpaces);
    for (uint32_t i = 0; i < numOutputSpaces; i++) {
        const char* spaceName = "UNKNOWN";
        switch (referenceSpaces[i]) {
            case XR_REFERENCE_SPACE_TYPE_VIEW: spaceName = "VIEW"; break;
            case XR_REFERENCE_SPACE_TYPE_LOCAL: spaceName = "LOCAL"; break;
            case XR_REFERENCE_SPACE_TYPE_STAGE: spaceName = "STAGE"; break;
            case XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR: spaceName = "LOCAL_FLOOR"; break;
            default: break;
        }
        ALOGV("  [%d] %s (%d)", i, spaceName, referenceSpaces[i]);

        if (referenceSpaces[i] == XR_REFERENCE_SPACE_TYPE_STAGE) {
            stageSupported = qtrue;
        }
        if (referenceSpaces[i] == XR_REFERENCE_SPACE_TYPE_LOCAL_FLOOR) {
            localFloorSupported = qtrue;
        }
    }
    ALOGV("Reference space support: LOCAL_FLOOR=%s, STAGE=%s",
          localFloorSupported ? "yes" : "no",
          stageSupported ? "yes" : "no");

    free(referenceSpaces);

    if (engine->appState.CurrentSpace == XR_NULL_HANDLE) {
        VR_Recenter(engine);
    }

    projections = (XrView*)(malloc(ovrMaxNumEyes * sizeof(XrView)));
    for (int eye = 0; eye < ovrMaxNumEyes; eye++) {
        memset(&projections[eye], 0, sizeof(XrView));
        projections[eye].type = XR_TYPE_VIEW;
    }

    ALOGV("VR_InitRenderer: creating renderer framebuffers");
    ovrRenderer_Create(
            engine->appState.Session,
            &engine->appState.Renderer,
            eyeW,
            eyeH);
    ALOGV("VR_InitRenderer: complete");
}

void VR_DestroyRenderer( engine_t* engine )
{
    ovrRenderer_Destroy(&engine->appState.Renderer);
    free(projections);
}

void VR_ReInitRenderer()
{
    VR_DestroyRenderer( VR_GetEngine() );
    VR_InitRenderer( VR_GetEngine() );
}

void VR_ClearFrameBuffer( int width, int height)
{
    glEnable( GL_SCISSOR_TEST );
    glViewport( 0, 0, width, height );

    if (Cvar_VariableIntegerValue("vr_thirdPersonSpectator"))
    {
        //Blood red.. ish
        glClearColor( 0.12f, 0.0f, 0.05f, 1.0f );
    }
    else
    {
        //Black
        glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    }

    glScissor( 0, 0, width, height );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glDisable( GL_FRAMEBUFFER_SRGB );

    glScissor( 0, 0, 0, 0 );
    glDisable( GL_SCISSOR_TEST );
}

void VR_DrawFrame( engine_t* engine ) {
	static int frameCount = 0;
	frameCount++;

	if (vr.weapon_zoomed) {
		vr.weapon_zoomLevel += 0.05;
		if (vr.weapon_zoomLevel > 2.5f)
			vr.weapon_zoomLevel = 2.5f;
	}
	else {
		//Zoom back out quicker
		vr.weapon_zoomLevel -= 0.25f;
		if (vr.weapon_zoomLevel < 1.0f)
			vr.weapon_zoomLevel = 1.0f;
	}

    static GLboolean stageBoundsDirty = GL_TRUE;
    if (ovrApp_HandleXrEvents(&engine->appState)) {
        VR_Recenter(engine);
    }
    if (engine->appState.SessionActive == GL_FALSE) {
        return;
    }

    if (stageBoundsDirty) {
        VR_UpdateStageBounds(&engine->appState);
        stageBoundsDirty = GL_FALSE;
    }

    // During loading states, let VR_PrepareLoadingFrame/VR_SubmitLoadingFrame handle
    // the frame lifecycle. This allows each SCR_UpdateScreen() call during loading
    // to submit its own VR frame, showing loading progress.
    if (clc.state == CA_LOADING || clc.state == CA_PRIMED) {
        Com_Frame();
        return;
    }

    // NOTE: OpenXR does not use the concept of frame indices. Instead,
    // XrWaitFrame returns the predicted display time.
    XrFrameWaitInfo waitFrameInfo = {};
    waitFrameInfo.type = XR_TYPE_FRAME_WAIT_INFO;
    waitFrameInfo.next = NULL;

    XrFrameState frameState = {};
    frameState.type = XR_TYPE_FRAME_STATE;
    frameState.next = NULL;

    OXR(xrWaitFrame(engine->appState.Session, &waitFrameInfo, &frameState));
    engine->predictedDisplayTime = frameState.predictedDisplayTime;

    // Get the HMD pose, predicted for the middle of the time period during which
    // the new eye images will be displayed. The number of frames predicted ahead
    // depends on the pipeline depth of the engine and the synthesis rate.
    // The better the prediction, the less black will be pulled in at the edges.
    XrFrameBeginInfo beginFrameDesc = {};
    beginFrameDesc.type = XR_TYPE_FRAME_BEGIN_INFO;
    beginFrameDesc.next = NULL;
    OXR(xrBeginFrame(engine->appState.Session, &beginFrameDesc));

    XrViewLocateInfo projectionInfo = {};
    projectionInfo.type = XR_TYPE_VIEW_LOCATE_INFO;
    projectionInfo.viewConfigurationType = engine->appState.ViewportConfig.viewConfigurationType;
    projectionInfo.displayTime = frameState.predictedDisplayTime;
    projectionInfo.space = engine->appState.CurrentSpace;

    XrViewState viewState = {XR_TYPE_VIEW_STATE, NULL};

    uint32_t projectionCapacityInput = ovrMaxNumEyes;
    uint32_t projectionCountOutput = projectionCapacityInput;

    OXR(xrLocateViews(
            engine->appState.Session,
            &projectionInfo,
            &viewState,
            projectionCapacityInput,
            &projectionCountOutput,
            projections));
    //

    XrFovf fov = {};
    XrPosef invViewTransform[2];
    for (int eye = 0; eye < ovrMaxNumEyes; eye++) {
        invViewTransform[eye] = projections[eye].pose;

        fov.angleLeft += projections[eye].fov.angleLeft / 2.0f;
        fov.angleRight += projections[eye].fov.angleRight / 2.0f;
        fov.angleUp += projections[eye].fov.angleUp / 2.0f;
        fov.angleDown += projections[eye].fov.angleDown / 2.0f;
    }
    vr.fov_x = (fabs(fov.angleLeft) + fabs(fov.angleRight)) * 180.0f / M_PI;
    vr.fov_y = (fabs(fov.angleUp) + fabs(fov.angleDown)) * 180.0f / M_PI;

    // Update HMD and controllers
    IN_VRUpdateHMD( invViewTransform[0] );
    IN_VRUpdateControllers( frameState.predictedDisplayTime );
    IN_VRSyncActions();

    //Projection used for drawing HUD models etc
    float hudScale = M_PI * 15.0f / 180.0f;
    const ovrMatrix4f monoVRMatrix = ovrMatrix4f_CreateProjectionFov(
            -hudScale, hudScale, hudScale, -hudScale, 1.0f, 0.0f );
    const ovrMatrix4f projectionMatrix = ovrMatrix4f_CreateProjectionFov(
            fov.angleLeft / vr.weapon_zoomLevel,
            fov.angleRight / vr.weapon_zoomLevel,
            fov.angleUp / vr.weapon_zoomLevel,
            fov.angleDown / vr.weapon_zoomLevel,
            1.0f, 0.0f );

    engine->appState.LayerCount = 0;
    memset(engine->appState.Layers, 0, sizeof(ovrCompositorLayer_Union) * ovrMaxLayerCount);

    ovrFramebuffer* frameBuffer = &engine->appState.Renderer.FrameBuffer;

    // Acquire swapchain image FIRST to get the correct index
    ovrFramebuffer_Acquire(frameBuffer);

    // Get the swapchain index (set by xrAcquireSwapchainImage in Acquire)
    int swapchainIndex = frameBuffer->TextureSwapChainIndex;
    int glFramebuffer = frameBuffer->FrameBuffers[swapchainIndex];
    re.SetVRHeadsetParms(projectionMatrix.M, monoVRMatrix.M, glFramebuffer);

    ovrFramebuffer_SetCurrent(frameBuffer);
    VR_ClearFrameBuffer(frameBuffer->ColorSwapChain.Width, frameBuffer->ColorSwapChain.Height);
    Com_Frame();

    // Clear the alpha channel, other way OpenXR would not transfer the framebuffer fully
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    ovrFramebuffer_Resolve(frameBuffer);
    ovrFramebuffer_Release(frameBuffer);
    ovrFramebuffer_SetNone();

    XrCompositionLayerProjectionView projection_layer_elements[2] = {};
    if (!VR_useScreenLayer() && !(cl.snap.ps.pm_flags & PMF_FOLLOW && vr.follow_mode == VRFM_FIRSTPERSON)) {
        // Note: menuYaw is now captured on transition to virtual screen mode, not here

        if (fullscreenMode) {
            VR_ReInitRenderer();
            fullscreenMode = qfalse;
        }

        for (int eye = 0; eye < ovrMaxNumEyes; eye++) {
            ovrFramebuffer* frameBuffer = &engine->appState.Renderer.FrameBuffer;

            memset(&projection_layer_elements[eye], 0, sizeof(XrCompositionLayerProjectionView));
            projection_layer_elements[eye].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
            projection_layer_elements[eye].pose = invViewTransform[eye];
            projection_layer_elements[eye].fov = fov;

            memset(&projection_layer_elements[eye].subImage, 0, sizeof(XrSwapchainSubImage));
            projection_layer_elements[eye].subImage.swapchain = frameBuffer->ColorSwapChain.Handle;
            projection_layer_elements[eye].subImage.imageRect.offset.x = 0;
            projection_layer_elements[eye].subImage.imageRect.offset.y = 0;
            projection_layer_elements[eye].subImage.imageRect.extent.width = frameBuffer->ColorSwapChain.Width;
            projection_layer_elements[eye].subImage.imageRect.extent.height = frameBuffer->ColorSwapChain.Height;
            projection_layer_elements[eye].subImage.imageArrayIndex = eye;
        }

        XrCompositionLayerProjection projection_layer = {};
        projection_layer.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
        projection_layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
        projection_layer.layerFlags |= XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
        projection_layer.space = engine->appState.CurrentSpace;
        projection_layer.viewCount = ovrMaxNumEyes;
        projection_layer.views = projection_layer_elements;

        engine->appState.Layers[engine->appState.LayerCount++].Projection = projection_layer;
    } else {
        // Capture menuYaw on FIRST FRAME of virtual screen mode (not during gameplay)
        if (!fullscreenMode) {
            vr.menuYaw = vr.hmdorientation[YAW];
        }
        fullscreenMode = qtrue;

        // Build the cylinder layer
        int width = engine->appState.Renderer.FrameBuffer.ColorSwapChain.Width;
        int height = engine->appState.Renderer.FrameBuffer.ColorSwapChain.Height;
        XrCompositionLayerCylinderKHR cylinder_layer = {};
        cylinder_layer.type = XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR;
        cylinder_layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
        cylinder_layer.space = engine->appState.CurrentSpace;
        cylinder_layer.eyeVisibility = XR_EYE_VISIBILITY_BOTH;
        memset(&cylinder_layer.subImage, 0, sizeof(XrSwapchainSubImage));
        cylinder_layer.subImage.swapchain = engine->appState.Renderer.FrameBuffer.ColorSwapChain.Handle;
        cylinder_layer.subImage.imageRect.offset.x = 0;
        cylinder_layer.subImage.imageRect.offset.y = 0;
        cylinder_layer.subImage.imageRect.extent.width = width;
        cylinder_layer.subImage.imageRect.extent.height = height;
        cylinder_layer.subImage.imageArrayIndex = 0;
        const XrVector3f axis = {0.0f, 1.0f, 0.0f};
        XrVector3f pos = {
                invViewTransform[0].position.x - sin(radians(vr.menuYaw)) * 6.0f,
                -0.25f,
                invViewTransform[0].position.z - cos(radians(vr.menuYaw)) * 6.0f
        };
        cylinder_layer.pose.orientation = XrQuaternionf_CreateFromVectorAngle(axis, radians(vr.menuYaw));

        // Screen curvature: 1.0 = very curved, 0.0 = nearly flat
        float curvature = vr_screenCurvature ? vr_screenCurvature->value : 0.5f;
        const float refRadius = 8.0f;
        const float refCentralAngle = MATH_PI * 0.5f;
        const float arcLength = refRadius * refCentralAngle;
        const float refDistance = 4.0f;

        // Vary radius, adjust centralAngle to keep arc length constant,
        // and adjust distance to keep surface at same position
        float radius = 4.0f + (1.0f - curvature) * 12.0f;
        float centralAngle = arcLength / radius;
        float axisDistance = refDistance + (refRadius - radius);

        pos.x = invViewTransform[0].position.x - sin(radians(vr.menuYaw)) * axisDistance;
        pos.z = invViewTransform[0].position.z - cos(radians(vr.menuYaw)) * axisDistance;
        cylinder_layer.pose.position = pos;

        cylinder_layer.radius = radius;
        cylinder_layer.centralAngle = centralAngle;
        cylinder_layer.aspectRatio = width / (float)height / 0.75f;

        engine->appState.Layers[engine->appState.LayerCount++].Cylinder = cylinder_layer;
    }

    // Compose the layers for this frame.
    const XrCompositionLayerBaseHeader* layers[ovrMaxLayerCount] = {};
    for (int i = 0; i < engine->appState.LayerCount; i++) {
        layers[i] = (const XrCompositionLayerBaseHeader*)&engine->appState.Layers[i];
    }

    XrFrameEndInfo endFrameInfo = {};
    endFrameInfo.type = XR_TYPE_FRAME_END_INFO;
    endFrameInfo.displayTime = frameState.predictedDisplayTime;
    endFrameInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
    endFrameInfo.layerCount = engine->appState.LayerCount;
    endFrameInfo.layers = layers;

    OXR(xrEndFrame(engine->appState.Session, &endFrameInfo));
    // Note: Do NOT manually increment TextureSwapChainIndex here
    // OpenXR manages the swapchain index via xrAcquireSwapchainImage in ovrFramebuffer_Acquire

    if (needRecenter)
    {
        VR_Recenter(engine);
        needRecenter = qfalse;
    }
}

// Track loading frame state for two-phase submit
static qboolean loadingFrameStarted = qfalse;
static XrTime loadingFrameDisplayTime = 0;

int VR_PrepareLoadingFrame( engine_t* engine )
{
	// Only prepare frames during loading states
	if (!engine || (clc.state != CA_LOADING && clc.state != CA_PRIMED))
	{
		return qfalse;
	}

	// Handle XR events and check session is active
	if (ovrApp_HandleXrEvents(&engine->appState)) {
		VR_Recenter(engine);
	}
	if (engine->appState.SessionActive == GL_FALSE) {
		return qfalse;
	}

	// Already started a frame? Don't start another
	if (loadingFrameStarted) {
		return qfalse;
	}

	ovrFramebuffer* frameBuffer = &engine->appState.Renderer.FrameBuffer;
	int width = frameBuffer->ColorSwapChain.Width;
	int height = frameBuffer->ColorSwapChain.Height;

	// Begin OpenXR frame
	XrFrameWaitInfo waitFrameInfo = {XR_TYPE_FRAME_WAIT_INFO};
	XrFrameState frameState = {XR_TYPE_FRAME_STATE};
	OXR(xrWaitFrame(engine->appState.Session, &waitFrameInfo, &frameState));
	engine->predictedDisplayTime = frameState.predictedDisplayTime;
	loadingFrameDisplayTime = frameState.predictedDisplayTime;

	XrFrameBeginInfo beginFrameDesc = {XR_TYPE_FRAME_BEGIN_INFO};
	OXR(xrBeginFrame(engine->appState.Session, &beginFrameDesc));

	// Acquire swapchain and set up renderer to draw to it
	ovrFramebuffer_Acquire(frameBuffer);

	// Set up projection matrices for loading screen
	float fov_x = vr.fov_x > 0 ? vr.fov_x : 90.0f;
	float fov_y = vr.fov_y > 0 ? vr.fov_y : 90.0f;
	float hudScale = M_PI * 15.0f / 180.0f;

	const ovrMatrix4f projectionMatrix = ovrMatrix4f_CreateProjectionFov(
		-fov_x / 2.0f * M_PI / 180.0f, fov_x / 2.0f * M_PI / 180.0f,
		fov_y / 2.0f * M_PI / 180.0f, -fov_y / 2.0f * M_PI / 180.0f,
		1.0f, 0.0f );
	const ovrMatrix4f monoVRMatrix = ovrMatrix4f_CreateProjectionFov(
		-hudScale, hudScale, hudScale, -hudScale, 1.0f, 0.0f );

	int swapchainIndex = frameBuffer->TextureSwapChainIndex;
	int glFramebuffer = frameBuffer->FrameBuffers[swapchainIndex];
	re.SetVRHeadsetParms(projectionMatrix.M, monoVRMatrix.M, glFramebuffer);

	ovrFramebuffer_SetCurrent(frameBuffer);
	VR_ClearFrameBuffer(width, height);

	loadingFrameStarted = qtrue;
	return qtrue;
}

int VR_SubmitLoadingFrame( engine_t* engine )
{
	// Only submit frames during loading states
	if (!engine || (clc.state != CA_LOADING && clc.state != CA_PRIMED))
	{
		loadingFrameStarted = qfalse;
		return qfalse;
	}

	// If we didn't start a loading frame, nothing to submit
	if (!loadingFrameStarted) {
		return qfalse;
	}

	if (engine->appState.SessionActive == GL_FALSE) {
		loadingFrameStarted = qfalse;
		return qfalse;
	}

	ovrFramebuffer* frameBuffer = &engine->appState.Renderer.FrameBuffer;
	int width = frameBuffer->ColorSwapChain.Width;
	int height = frameBuffer->ColorSwapChain.Height;

	// Clear alpha channel so OpenXR transfers framebuffer correctly
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	// Release swapchain image
	ovrFramebuffer_Resolve(frameBuffer);
	ovrFramebuffer_Release(frameBuffer);
	ovrFramebuffer_SetNone();

	// Build cylinder layer for loading screen
	XrCompositionLayerCylinderKHR cylinder_layer = {XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR};
	cylinder_layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
	cylinder_layer.space = engine->appState.CurrentSpace;
	cylinder_layer.eyeVisibility = XR_EYE_VISIBILITY_BOTH;
	cylinder_layer.subImage.swapchain = frameBuffer->ColorSwapChain.Handle;
	cylinder_layer.subImage.imageRect.extent.width = width;
	cylinder_layer.subImage.imageRect.extent.height = height;
	const XrVector3f axis = {0.0f, 1.0f, 0.0f};
	cylinder_layer.pose.orientation = XrQuaternionf_CreateFromVectorAngle(axis, radians(vr.menuYaw));

	// Screen curvature: 1.0 = very curved, 0.0 = nearly flat
	float curvature = vr_screenCurvature ? vr_screenCurvature->value : 0.5f;
	const float refRadius = 8.0f;
	const float refCentralAngle = MATH_PI * 0.5f;
	const float arcLength = refRadius * refCentralAngle;
	const float refDistance = 4.0f;

	float loadingRadius = 4.0f + (1.0f - curvature) * 12.0f;
	float loadingCentralAngle = arcLength / loadingRadius;
	float loadingDistance = refDistance + (refRadius - loadingRadius);

	cylinder_layer.pose.position = (XrVector3f){-sin(radians(vr.menuYaw)) * loadingDistance, -0.25f, -cos(radians(vr.menuYaw)) * loadingDistance};
	cylinder_layer.radius = loadingRadius;
	cylinder_layer.centralAngle = loadingCentralAngle;
	cylinder_layer.aspectRatio = width / (float)height / 0.75f;

	const XrCompositionLayerBaseHeader* layers[] = {(const XrCompositionLayerBaseHeader*)&cylinder_layer};

	XrFrameEndInfo endFrameInfo = {XR_TYPE_FRAME_END_INFO};
	endFrameInfo.displayTime = loadingFrameDisplayTime;
	endFrameInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
	endFrameInfo.layerCount = 1;
	endFrameInfo.layers = layers;
	OXR(xrEndFrame(engine->appState.Session, &endFrameInfo));

	loadingFrameStarted = qfalse;
	return qtrue;
}
