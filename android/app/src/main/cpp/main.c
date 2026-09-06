#include <jni.h>
#include <memory.h>
#include <string.h>

#include <android/log.h>

#include <client/keycodes.h>
#include <qcommon/q_shared.h>
#include <qcommon/qcommon.h>
#include <vrcommon/vr_base.h>
#include <vrcommon/vr_input.h>
#include <vrcommon/vr_instance.h>
#include <vrcommon/vr_renderer.h>
#include <unistd.h>

#include <SDL.h>

extern void CON_LogcatFn( void (*LogcatFn)( const char* message ) );
extern void Sys_Android_SetJNIEnv( JavaVM *vm, jobject activity );


#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Quake3", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Quake3", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "Quake3", __VA_ARGS__))
#define LOGF(...) ((void)__android_log_print(ANDROID_LOG_FATAL, "Quake3", __VA_ARGS__))

static JNIEnv* g_Env = NULL;
static JavaVM* g_JavaVM = NULL;
static jobject g_ActivityObject = NULL;
static qboolean g_HasFocus = qtrue;

JNIEXPORT void JNICALL Java_io_ernie_trinity_MainActivity_nativeCreate(JNIEnv* env, jclass cls, jobject thisObject)
{
    g_ActivityObject = (*env)->NewGlobalRef(env, thisObject);
}

JNIEXPORT void JNICALL Java_io_ernie_trinity_MainActivity_nativeFocusChanged(JNIEnv *env, jclass clazz, jboolean focus)
{
    g_HasFocus = focus;
}

JNIEXPORT void JNICALL Java_io_ernie_trinity_MainActivity_nativeKey(JNIEnv *env, jclass clazz, jint keycode, jint action)
{
	if (action == 0)
	{
		Com_QueueEvent( 0, SE_CHAR, keycode, qtrue, 0, NULL );
	}
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    g_JavaVM = vm;
    if ((*g_JavaVM)->GetEnv(g_JavaVM, (void**) &g_Env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }

    return JNI_VERSION_1_4;
}

static void ioq3_logfn(const char* msg)
{
	LOGI("%s", msg);
}

int main(int argc, char* argv[]) {
	// Attach current thread to Java VM
	JNIEnv* env = NULL;
	(*g_JavaVM)->AttachCurrentThread(g_JavaVM, &env, NULL);

	// Set Android context for OpenXR initialization
	VR_SetAndroidContext(g_JavaVM, g_ActivityObject);

	// Set JNI context for APK installer
	Sys_Android_SetJNIEnv(g_JavaVM, g_ActivityObject);

	// Initialize VR engine
	VR_Engine* engine = VR_Init();
	if (!engine) {
		LOGE("VR_Init failed!");
		return -1;
	}

	// Get resolution for cached values
	int width, height;
	VR_GetResolution(engine, &width, &height);

	CON_LogcatFn(&ioq3_logfn);

    char *args = (char*)getenv("commandline");

    Com_Init(args);
    NET_Init();

	LOGI("Calling VR_EnterVR");
	VR_EnterVR(engine);
	LOGI("Calling VR_InitRenderer");
	VR_InitRenderer(engine);
	LOGI("Calling VR_InitSessionInput");
	VR_InitSessionInput(engine);
	LOGI("VR initialization complete, entering main loop");

	qboolean hasFocus = qtrue;
	qboolean paused = qfalse;
	while (1) {
		if (hasFocus != g_HasFocus) {
			hasFocus = g_HasFocus;
			if (!hasFocus) {
				// Lost focus - send ESC to pause
				Com_QueueEvent( Sys_Milliseconds(), SE_KEY, K_ESCAPE, qtrue, 0, NULL );
				paused = qtrue;
			} else if (hasFocus && paused) {
				// Regained focus - send ESC to unpause
				Com_QueueEvent( Sys_Milliseconds(), SE_KEY, K_ESCAPE, qtrue, 0, NULL );
				paused = qfalse;
			}
		}

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			LOGI("Received SDL Event: %d", event.type);
			switch (event.type)
			{
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					VR_EnterVR(engine);
					break;

				case SDL_WINDOWEVENT_FOCUS_LOST:
					VR_LeaveVR(engine);
					break;
			}
		}

		VR_ProcessFrame(engine);
	}

	VR_LeaveVR(engine);
	VR_DestroyRenderer(engine);

	Com_Shutdown();
	VR_Destroy(engine);

	return 0;
}
