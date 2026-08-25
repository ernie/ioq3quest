#ifndef __VR_INSTANCE
#define __VR_INSTANCE

#include "vr_types.h"
#include "vr_safe_types.h"

#if __ANDROID__
// Must be called before VR_Init() on Android to provide Java context
void VR_SetAndroidContext(void* javaVM, void* activityObject);
// Initialize the Android OpenXR loader; idempotent, safe to call multiple times
XrResult VR_InitializeLoaderAndroid(void);
#endif

XrResult VR_CreateInstance(const char* app_name, XrVersion api_version, uint32_t extensionsCount, const char* const* extensions, XrInstance* instance);
XrResult VR_GetHMDSystem(XrInstance instance, XrSystemId* systemId);
XrResult VR_GetSystemProperties(XrInstance instance, XrSystemId systemId, VR_SystemProperties* systemProperties);

#endif
