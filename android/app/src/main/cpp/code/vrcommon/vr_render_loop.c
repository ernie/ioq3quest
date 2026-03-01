#include "vr_render_loop.h"

#include <string.h>
#include <math.h>

#include "vr_macros.h"
#include "vr_clientinfo.h"
#include "vr_gameplay.h"
#include "vr_cvars.h"
#include "../vrvk/vr_vk_types.h"
#include "common/xr_linear.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static inline float radians(float degrees) {
	return degrees * (float)M_PI / 180.0f;
}

XrFrameState VR_WaitFrame(XrSession session)
{
	XrFrameWaitInfo waitFrameInfo = {};
	waitFrameInfo.type = XR_TYPE_FRAME_WAIT_INFO;
	waitFrameInfo.next = NULL;

	XrFrameState frameState = {};
	frameState.type = XR_TYPE_FRAME_STATE;
	frameState.next = NULL;

	XR_CHECK(
		xrWaitFrame(session, &waitFrameInfo, &frameState),
		"Failed to wait for XR frame");

	return frameState;
}

void VR_BeginFrame(XrSession session)
{
	XrFrameBeginInfo beginFrameDesc = {};
	beginFrameDesc.type = XR_TYPE_FRAME_BEGIN_INFO;
	beginFrameDesc.next = NULL;
	XR_CHECK(
		xrBeginFrame(session, &beginFrameDesc),
		"Failed to begin XR frame");
}

XrViewState VR_LocateViews(XrSession session, XrTime predictedDisplayTime, XrSpace space, XrView* views, uint32_t* viewCount)
{
	XrViewLocateInfo projectionInfo = {};
	projectionInfo.type = XR_TYPE_VIEW_LOCATE_INFO;
	projectionInfo.next = NULL;
	projectionInfo.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	projectionInfo.displayTime = predictedDisplayTime;
	projectionInfo.space = space;

	XrViewState viewState = {0};
	viewState.type = XR_TYPE_VIEW_STATE;
	viewState.next = NULL;

	views[0].type = views[1].type = XR_TYPE_VIEW;
	views[0].next = views[1].next = NULL;

	XR_CHECK(
		xrLocateViews(
			session,
			&projectionInfo,
			&viewState,
			*viewCount,
			viewCount,
			views),
		"Failed to locate XR views");
	
	return viewState;
}

void VR_EndFrame(XrSession session, VR_SwapchainInfos* swapchains, XrView* views, uint32_t viewCount, XrFovf fov, XrSpace worldSpace, XrSpace viewSpace, XrTime predictedDisplayTime)
{
	extern vr_clientinfo_t vr;
	extern cvar_t* vr_screenCurvature;

	// Check if we should render to virtual screen (menus, spectator mode)
	qboolean useVirtualScreen = VR_Gameplay_ShouldRenderInVirtualScreen();

	XrCompositionLayerProjectionView projection_layer_elements[2] = {};

	for (uint32_t view = 0; view < viewCount; view++)
	{
		memset(&projection_layer_elements[view], 0, sizeof(XrCompositionLayerProjectionView));
		projection_layer_elements[view].type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
		projection_layer_elements[view].pose = views[view].pose;

		// Weapon zoom: cyclopean rendering — both eyes rendered with the same
		// averaged symmetric projection from center viewpoint, so tell the
		// compositor both eyes share the same (averaged) FOV.  This prevents
		// the per-eye asymmetry from shifting the identical images apart.
		projection_layer_elements[view].fov = vr.weapon_zoomed ? fov : views[view].fov;

		memset(&projection_layer_elements[view].subImage, 0, sizeof(XrSwapchainSubImage));
		projection_layer_elements[view].subImage.swapchain = swapchains->color.swapchain;
		projection_layer_elements[view].subImage.imageRect.offset.x = 0;
		projection_layer_elements[view].subImage.imageRect.offset.y = 0;
		projection_layer_elements[view].subImage.imageRect.extent.width = swapchains->color.width;
		projection_layer_elements[view].subImage.imageRect.extent.height = swapchains->color.height;
		projection_layer_elements[view].subImage.imageArrayIndex = view;
	}

	XrCompositionLayerProjection projection_layer = {};
	projection_layer.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION;
	projection_layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
	projection_layer.layerFlags |= XR_COMPOSITION_LAYER_CORRECT_CHROMATIC_ABERRATION_BIT;
	projection_layer.space = worldSpace;
	projection_layer.viewCount = viewCount;
	projection_layer.views = projection_layer_elements;

	// Cylinder layer for virtual screen (menus, spectator mode)
	// Uses the main color swapchain rendered content, displayed on a curved surface
	XrCompositionLayerCylinderKHR cylinder_layer = {};
	if (useVirtualScreen && viewCount > 0)
	{
		int width = swapchains->color.width;
		int height = swapchains->color.height;

		cylinder_layer.type = XR_TYPE_COMPOSITION_LAYER_CYLINDER_KHR;
		cylinder_layer.layerFlags = XR_COMPOSITION_LAYER_BLEND_TEXTURE_SOURCE_ALPHA_BIT;
		cylinder_layer.space = worldSpace;
		cylinder_layer.eyeVisibility = XR_EYE_VISIBILITY_BOTH;

		memset(&cylinder_layer.subImage, 0, sizeof(XrSwapchainSubImage));
		cylinder_layer.subImage.swapchain = swapchains->color.swapchain;
		cylinder_layer.subImage.imageRect.offset.x = 0;
		cylinder_layer.subImage.imageRect.offset.y = 0;
		cylinder_layer.subImage.imageRect.extent.width = width;
		cylinder_layer.subImage.imageRect.extent.height = height;
		cylinder_layer.subImage.imageArrayIndex = 0;  // Cylinder uses single image, not array

		// Position cylinder in front of player at menuYaw direction
		const XrVector3f axis = {0.0f, 1.0f, 0.0f};

		// Screen curvature: 1.0 = very curved, 0.0 = nearly flat
		float curvature = vr_screenCurvature ? vr_screenCurvature->value : 0.5f;
		const float refRadius = 8.0f;
		const float refCentralAngle = (float)M_PI * 0.5f;
		const float arcLength = refRadius * refCentralAngle;
		const float refDistance = 4.0f;

		// Vary radius, adjust centralAngle to keep arc length constant,
		// and adjust distance to keep surface at same position
		float radius = 4.0f + (1.0f - curvature) * 12.0f;
		float centralAngle = arcLength / radius;
		float axisDistance = refDistance + (refRadius - radius);

		XrVector3f pos = {
			views[0].pose.position.x - sinf(radians(vr.menuYaw)) * axisDistance,
			-0.25f,
			views[0].pose.position.z - cosf(radians(vr.menuYaw)) * axisDistance
		};
		cylinder_layer.pose.position = pos;
		XrQuaternionf_CreateFromAxisAngle(&cylinder_layer.pose.orientation, &axis, radians(vr.menuYaw));

		cylinder_layer.radius = radius;
		cylinder_layer.centralAngle = centralAngle;
		cylinder_layer.aspectRatio = width / (float)height / 0.75f;
	}

	// Submit layers
	const XrCompositionLayerBaseHeader* layers[2];
	int layerCount = 0;

	if (useVirtualScreen && viewCount > 0)
	{
		// Virtual screen mode: use cylinder layer instead of projection
		layers[layerCount++] = (const XrCompositionLayerBaseHeader*)&cylinder_layer;
	}
	else
	{
		// Normal gameplay: use projection layer
		if (viewCount > 0)
		{
			layers[layerCount++] = (const XrCompositionLayerBaseHeader*)&projection_layer;
		}
	}

	XrFrameEndInfo endFrameInfo = {};
	endFrameInfo.type = XR_TYPE_FRAME_END_INFO;
	endFrameInfo.displayTime = predictedDisplayTime;
	endFrameInfo.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
	endFrameInfo.layerCount = layerCount;
	endFrameInfo.layers = layerCount > 0 ? layers : NULL;

	XR_CHECK(
		xrEndFrame(session, &endFrameInfo),
		"Failed to end XR frame");
}
