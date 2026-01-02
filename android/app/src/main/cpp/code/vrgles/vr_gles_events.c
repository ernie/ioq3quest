/*
 * vr_gles_events.c - OpenXR event handling for GL ES
 *
 * Implements the event handling API declared in vrcommon/vr_events.h.
 * Contains ovrApp lifecycle management functions.
 * Extracted from vr_types.c
 */

#include "vr_gles_types.h"
#include "../vrcommon/vr_events.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

/*
================================================================================

ovrApp

================================================================================
*/

void ovrApp_Clear(ovrApp* app) {
    app->Focused = false;
    app->Instance = XR_NULL_HANDLE;
    app->Session = XR_NULL_HANDLE;
    memset(&app->ViewportConfig, 0, sizeof(XrViewConfigurationProperties));
    memset(&app->ViewConfigurationView, 0, ovrMaxNumEyes * sizeof(XrViewConfigurationView));
    app->SystemId = XR_NULL_SYSTEM_ID;
    app->HeadSpace = XR_NULL_HANDLE;
    app->StageSpace = XR_NULL_HANDLE;
    app->FakeStageSpace = XR_NULL_HANDLE;
    app->CurrentSpace = XR_NULL_HANDLE;
    app->SessionActive = false;
    app->SupportedDisplayRefreshRates = NULL;
    app->RequestedDisplayRefreshRateIndex = 0;
    app->NumSupportedDisplayRefreshRates = 0;
    app->pfnGetDisplayRefreshRate = NULL;
    app->pfnRequestDisplayRefreshRate = NULL;
    app->SwapInterval = 1;
    memset(app->Layers, 0, sizeof(ovrCompositorLayer_Union) * ovrMaxLayerCount);
    app->LayerCount = 0;
    app->MainThreadTid = 0;
    app->RenderThreadTid = 0;

    ovrRenderer_Clear(&app->Renderer);
}

void ovrApp_Destroy(ovrApp* app) {
    if (app->SupportedDisplayRefreshRates != NULL) {
        free(app->SupportedDisplayRefreshRates);
    }

    ovrApp_Clear(app);
}

void ovrApp_HandleSessionStateChanges(ovrApp* app, XrSessionState state) {
    if (state == XR_SESSION_STATE_READY) {
        assert(app->SessionActive == false);

        ALOGV("ovrApp_HandleSessionStateChanges: READY state, calling xrBeginSession");

        XrSessionBeginInfo sessionBeginInfo;
        memset(&sessionBeginInfo, 0, sizeof(sessionBeginInfo));
        sessionBeginInfo.type = XR_TYPE_SESSION_BEGIN_INFO;
        sessionBeginInfo.next = NULL;
        sessionBeginInfo.primaryViewConfigurationType = app->ViewportConfig.viewConfigurationType;

        ALOGV("xrBeginSession: viewConfigurationType=%d", sessionBeginInfo.primaryViewConfigurationType);

        XrResult result;
        OXR(result = xrBeginSession(app->Session, &sessionBeginInfo));

        ALOGV("xrBeginSession result: %d", result);
        app->SessionActive = (result == XR_SUCCESS);
        ALOGV("SessionActive set to: %d", app->SessionActive);

        // Set session state once we have entered VR mode and have a valid session object.
        if (app->SessionActive) {
            ALOGV("Session is active, setting performance levels");

            XrPerfSettingsLevelEXT cpuPerfLevel = XR_PERF_SETTINGS_LEVEL_BOOST_EXT;
            XrPerfSettingsLevelEXT gpuPerfLevel = XR_PERF_SETTINGS_LEVEL_BOOST_EXT;

            PFN_xrPerfSettingsSetPerformanceLevelEXT pfnPerfSettingsSetPerformanceLevelEXT = NULL;
            OXR(xrGetInstanceProcAddr(
                    app->Instance,
                    "xrPerfSettingsSetPerformanceLevelEXT",
                    (PFN_xrVoidFunction*)(&pfnPerfSettingsSetPerformanceLevelEXT)));

            if (pfnPerfSettingsSetPerformanceLevelEXT != NULL) {
                ALOGV("Setting CPU/GPU performance levels");
                OXR(pfnPerfSettingsSetPerformanceLevelEXT(
                        app->Session, XR_PERF_SETTINGS_DOMAIN_CPU_EXT, cpuPerfLevel));
                OXR(pfnPerfSettingsSetPerformanceLevelEXT(
                        app->Session, XR_PERF_SETTINGS_DOMAIN_GPU_EXT, gpuPerfLevel));
            } else {
                ALOGV("Performance settings extension not available");
            }

            PFN_xrSetAndroidApplicationThreadKHR pfnSetAndroidApplicationThreadKHR = NULL;
            OXR(xrGetInstanceProcAddr(
                    app->Instance,
                    "xrSetAndroidApplicationThreadKHR",
                    (PFN_xrVoidFunction*)(&pfnSetAndroidApplicationThreadKHR)));

            if (pfnSetAndroidApplicationThreadKHR != NULL) {
                ALOGV("Setting Android application threads");
                OXR(pfnSetAndroidApplicationThreadKHR(
                        app->Session, XR_ANDROID_THREAD_TYPE_APPLICATION_MAIN_KHR, app->MainThreadTid));
                OXR(pfnSetAndroidApplicationThreadKHR(
                        app->Session, XR_ANDROID_THREAD_TYPE_RENDERER_MAIN_KHR, app->RenderThreadTid));
            } else {
                ALOGV("Android thread settings extension not available");
            }

            ALOGV("Session setup complete");
        }
    } else if (state == XR_SESSION_STATE_STOPPING) {
        assert(app->SessionActive);

        OXR(xrEndSession(app->Session));
        app->SessionActive = false;
    }
}

bool ovrApp_HandleXrEvents(ovrApp* app) {
    XrEventDataBuffer eventDataBuffer = {};
    bool recenter = false;

    static int pollCount = 0;
    pollCount++;

    // Poll for events
    for (;;) {
        XrEventDataBaseHeader* baseEventHeader = (XrEventDataBaseHeader*)(&eventDataBuffer);
        baseEventHeader->type = XR_TYPE_EVENT_DATA_BUFFER;
        baseEventHeader->next = NULL;
        XrResult r;
        OXR(r = xrPollEvent(app->Instance, &eventDataBuffer));
        if (r != XR_SUCCESS) {
            if (pollCount <= 10 || pollCount % 100 == 0) {
                ALOGV("ovrApp_HandleXrEvents[%d]: no more events, SessionActive=%d", pollCount, app->SessionActive);
            }
            break;
        }
        ALOGV("ovrApp_HandleXrEvents[%d]: got event type %d", pollCount, baseEventHeader->type);

        switch (baseEventHeader->type) {
            case XR_TYPE_EVENT_DATA_EVENTS_LOST:
                ALOGV("xrPollEvent: received XR_TYPE_EVENT_DATA_EVENTS_LOST event");
                break;
            case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
                const XrEventDataInstanceLossPending* instance_loss_pending_event =
                        (XrEventDataInstanceLossPending*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING event: time %f",
                        FromXrTime(instance_loss_pending_event->lossTime));
            } break;
            case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
                ALOGV("xrPollEvent: received XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED event");
                break;
            case XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT: {
                const XrEventDataPerfSettingsEXT* perf_settings_event =
                        (XrEventDataPerfSettingsEXT*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_PERF_SETTINGS_EXT event: type %d subdomain %d : level %d -> level %d",
                        perf_settings_event->type,
                        perf_settings_event->subDomain,
                        perf_settings_event->fromLevel,
                        perf_settings_event->toLevel);
            } break;
            case XR_TYPE_EVENT_DATA_DISPLAY_REFRESH_RATE_CHANGED_FB: {
                const XrEventDataDisplayRefreshRateChangedFB* refresh_rate_changed_event =
                        (XrEventDataDisplayRefreshRateChangedFB*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_DISPLAY_REFRESH_RATE_CHANGED_FB event: fromRate %f -> toRate %f",
                        refresh_rate_changed_event->fromDisplayRefreshRate,
                        refresh_rate_changed_event->toDisplayRefreshRate);
            } break;
            case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING: {
                XrEventDataReferenceSpaceChangePending* ref_space_change_event =
                        (XrEventDataReferenceSpaceChangePending*)(baseEventHeader);
                ALOGV(
                        "xrPollEvent: received XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING event: changed space: %d for session %p at time %f",
                        ref_space_change_event->referenceSpaceType,
                        (void*)ref_space_change_event->session,
                        FromXrTime(ref_space_change_event->changeTime));
                recenter = true;
            } break;
            case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
                const XrEventDataSessionStateChanged* session_state_changed_event =
                        (XrEventDataSessionStateChanged*)(baseEventHeader);

                const char* stateNames[] = {
                    "UNKNOWN", "IDLE", "READY", "SYNCHRONIZED", "VISIBLE",
                    "FOCUSED", "STOPPING", "LOSS_PENDING", "EXITING"
                };
                int stateIndex = session_state_changed_event->state;
                const char* stateName = (stateIndex >= 0 && stateIndex <= 8) ? stateNames[stateIndex] : "INVALID";

                ALOGV(
                        "xrPollEvent: SESSION_STATE_CHANGED: %s (%d) for session %p at time %f",
                        stateName,
                        session_state_changed_event->state,
                        (void*)session_state_changed_event->session,
                        FromXrTime(session_state_changed_event->time));

                switch (session_state_changed_event->state) {
                    case XR_SESSION_STATE_FOCUSED:
                        ALOGV("Session is now FOCUSED");
                        app->Focused = true;
                        break;
                    case XR_SESSION_STATE_VISIBLE:
                        ALOGV("Session is now VISIBLE (but not focused)");
                        app->Focused = false;
                        break;
                    case XR_SESSION_STATE_SYNCHRONIZED:
                        ALOGV("Session is now SYNCHRONIZED");
                        break;
                    case XR_SESSION_STATE_READY:
                    case XR_SESSION_STATE_STOPPING:
                        ovrApp_HandleSessionStateChanges(app, session_state_changed_event->state);
                        break;
                    default:
                        break;
                }
            } break;
            default:
                ALOGV("xrPollEvent: Unknown event");
                break;
        }
    }
    return recenter;
}
