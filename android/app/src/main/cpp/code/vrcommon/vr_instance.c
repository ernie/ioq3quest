#include "vr_instance.h"

#include <string.h>
#include <stdio.h>

#include "../qcommon/q_shared.h"

#include "vr_macros.h"

#if __ANDROID__
#include <openxr/openxr_platform.h>

// Android-specific: stored context for XR instance creation
static JavaVM* s_javaVM = NULL;
static jobject s_activityObject = NULL;
static qboolean s_loaderInitialized = qfalse;

void VR_SetAndroidContext(void* javaVM, void* activityObject)
{
	s_javaVM = (JavaVM*)javaVM;
	s_activityObject = (jobject)activityObject;
}

XrResult VR_InitializeLoaderAndroid(void)
{
	if (s_loaderInitialized) {
		return XR_SUCCESS;
	}
	s_loaderInitialized = qtrue;
	// On Android, we must initialize the OpenXR loader before creating an instance
	PFN_xrInitializeLoaderKHR xrInitializeLoaderKHR = NULL;
	XrResult result = xrGetInstanceProcAddr(
		XR_NULL_HANDLE,
		"xrInitializeLoaderKHR",
		(PFN_xrVoidFunction*)&xrInitializeLoaderKHR);

	if (result != XR_SUCCESS || xrInitializeLoaderKHR == NULL) {
		fprintf(stderr, "[OpenXR] xrInitializeLoaderKHR not available (result=%d)\n", result);
		// This might be OK on some platforms, continue anyway
		return XR_SUCCESS;
	}

	XrLoaderInitInfoAndroidKHR loaderInitInfo;
	memset(&loaderInitInfo, 0, sizeof(loaderInitInfo));
	loaderInitInfo.type = XR_TYPE_LOADER_INIT_INFO_ANDROID_KHR;
	loaderInitInfo.next = NULL;
	loaderInitInfo.applicationVM = s_javaVM;
	loaderInitInfo.applicationContext = s_activityObject;

	result = xrInitializeLoaderKHR((XrLoaderInitInfoBaseHeaderKHR*)&loaderInitInfo);
	if (result != XR_SUCCESS) {
		fprintf(stderr, "[OpenXR] xrInitializeLoaderKHR failed: %d\n", result);
	} else {
		fprintf(stderr, "[OpenXR] Android loader initialized successfully\n");
	}
	return result;
}
#endif

XrResult VR_CreateInstance(const char* app_name, XrVersion api_version, uint32_t extensionsCount, const char* const* extensions, XrInstance* instance)
{
#if __ANDROID__
	// Initialize Android loader first
	XrResult loaderResult = VR_InitializeLoaderAndroid();
	if (loaderResult != XR_SUCCESS) {
		return loaderResult;
	}
#endif

	XrApplicationInfo appInfo;
	memset(&appInfo, 0, sizeof(appInfo));
	Q_strncpyz(appInfo.applicationName, app_name, sizeof(appInfo.applicationName));
	appInfo.applicationVersion = 1;
	Q_strncpyz(appInfo.engineName, app_name, sizeof(appInfo.engineName));
	appInfo.engineVersion = 1;
	appInfo.apiVersion = api_version;

#if __ANDROID__
	// Android requires XrInstanceCreateInfoAndroidKHR to pass Java context
	XrInstanceCreateInfoAndroidKHR instanceCreateInfoAndroid;
	memset(&instanceCreateInfoAndroid, 0, sizeof(instanceCreateInfoAndroid));
	instanceCreateInfoAndroid.type = XR_TYPE_INSTANCE_CREATE_INFO_ANDROID_KHR;
	instanceCreateInfoAndroid.next = NULL;
	instanceCreateInfoAndroid.applicationVM = s_javaVM;
	instanceCreateInfoAndroid.applicationActivity = s_activityObject;
#endif

	XrInstanceCreateInfo instanceCreateInfo;
	memset(&instanceCreateInfo, 0, sizeof(instanceCreateInfo));
	instanceCreateInfo.type = XR_TYPE_INSTANCE_CREATE_INFO;
#if __ANDROID__
	instanceCreateInfo.next = &instanceCreateInfoAndroid;
#else
	instanceCreateInfo.next = NULL;
#endif
	instanceCreateInfo.createFlags = 0;
	instanceCreateInfo.applicationInfo = appInfo;
	instanceCreateInfo.enabledApiLayerCount = 0;
	instanceCreateInfo.enabledApiLayerNames = NULL;
	instanceCreateInfo.enabledExtensionCount = extensionsCount;
	instanceCreateInfo.enabledExtensionNames = extensions;

	return xrCreateInstance(&instanceCreateInfo, instance);
}

XrResult VR_GetHMDSystem(XrInstance instance, XrSystemId* systemId)
{
	XrSystemGetInfo systemGetInfo;
	memset(&systemGetInfo, 0, sizeof(systemGetInfo));
	systemGetInfo.type = XR_TYPE_SYSTEM_GET_INFO;
	systemGetInfo.next = NULL;
	systemGetInfo.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;

	return xrGetSystem(instance, &systemGetInfo, systemId);
}

// Graphics requirements are fetched via VR_Graphics_GetRequirements() in vrvk/vr_vk.c

XrResult VR_GetSystemProperties(XrInstance instance, XrSystemId systemId, VR_SystemProperties* systemProperties, VR_Bool queryEyeTrackedFoveation)
{
	systemProperties->SystemProperties.type = XR_TYPE_SYSTEM_PROPERTIES;
	systemProperties->SystemProperties.next = NULL;

	// Chain the eye tracked struct only when its extension is enabled; the runtime rejects unknown structs
	memset(&systemProperties->FoveationEyeTracked, 0, sizeof(systemProperties->FoveationEyeTracked));
	systemProperties->FoveationEyeTracked.type = XR_TYPE_SYSTEM_FOVEATION_EYE_TRACKED_PROPERTIES_META;
	systemProperties->FoveationEyeTracked.next = NULL;
	if (queryEyeTrackedFoveation)
	{
		systemProperties->SystemProperties.next = &systemProperties->FoveationEyeTracked;
	}

	XR_CHECK(
		xrGetSystemProperties(instance, systemId, &systemProperties->SystemProperties),
		"Failed to get SystemProperties");

	// Graphics requirements are fetched separately via VR_Graphics_GetRequirements()

	return XR_SUCCESS;
}
