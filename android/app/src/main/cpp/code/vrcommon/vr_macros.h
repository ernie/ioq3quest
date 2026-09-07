#ifndef __VR_MACROS
#define __VR_MACROS

#include <stdio.h>
#include <stdlib.h>

#include "vr_debug.h"

#ifdef USE_DEBUG_STACKTRACE
void print_stacktrace(void);
#define PRINT_STACKTRACE print_stacktrace()
#else
#define PRINT_STACKTRACE
#endif

// stderr goes nowhere on Android, so a failed check also lands in logcat
#ifdef __ANDROID__
#include <android/log.h>
#define VR_CHECK_LOG(...) __android_log_print(ANDROID_LOG_FATAL, "Trinity", __VA_ARGS__)
#else
#define VR_CHECK_LOG(...) ((void)0)
#endif

// Plain
#define CHECK(expr, msg)  \
	{                                                                                                                                                   \
		if (!(expr)) {                                                                                                                                    \
			fprintf(stderr, "[VR] Check failed:\n  Expression: %s\n  Message: %s\n", #expr, msg);                                                           \
			VR_CHECK_LOG("[VR] Check failed: %s: %s", #expr, msg);                                                                                          \
			PRINT_STACKTRACE;                                                                                                                               \
			exit(1);                                                                                                                                        \
		}                                                                                                                                                 \
	}


// OpenXR
#define XR_CHECK(expr, msg)                                                                                                                           \
	{                                                                                                                                                   \
		XrResult result = (expr);                                                                                                                         \
		if (!XR_SUCCEEDED(result)) {                                                                                                                      \
			const char* result_str = GetXRErrorString(result);                                                                                              \
			fprintf(stderr, "[OpenXR] Check failed:\n  Expression: %s\n  Result: %d (%s)\n  Message: %s\n", #expr, (int)result, result_str, msg);           \
			VR_CHECK_LOG("[OpenXR] Check failed: %s: %d (%s): %s", #expr, (int)result, result_str, msg);                                                    \
			PRINT_STACKTRACE;                                                                                                                               \
			exit(1);                                                                                                                                        \
		}                                                                                                                                                 \
	}

// GL
// TODO

#endif
