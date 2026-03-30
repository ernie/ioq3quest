#include <jni.h>
#include "../qcommon/q_shared.h"
#include "../qcommon/qcommon.h"

static JavaVM *s_JavaVM = NULL;
static jobject s_ActivityObject = NULL;

void Sys_Android_SetJNIEnv( JavaVM *vm, jobject activity )
{
	s_JavaVM = vm;
	s_ActivityObject = activity;
}

void Sys_InstallApk( const char *apkPath )
{
	JNIEnv *env;
	jclass cls;
	jmethodID mid;
	jstring jpath;

	if ( !s_JavaVM || !s_ActivityObject ) {
		Com_Printf( "Sys_InstallApk: JNI not initialized\n" );
		return;
	}

	if ( (*s_JavaVM)->AttachCurrentThread( s_JavaVM, &env, NULL ) != JNI_OK ) {
		Com_Printf( "Sys_InstallApk: failed to attach thread\n" );
		return;
	}

	cls = (*env)->GetObjectClass( env, s_ActivityObject );
	if ( !cls ) {
		Com_Printf( "Sys_InstallApk: failed to get Activity class\n" );
		return;
	}

	mid = (*env)->GetMethodID( env, cls, "installApk", "(Ljava/lang/String;)V" );
	if ( !mid ) {
		Com_Printf( "Sys_InstallApk: installApk method not found\n" );
		(*env)->DeleteLocalRef( env, cls );
		return;
	}

	jpath = (*env)->NewStringUTF( env, apkPath );
	(*env)->CallVoidMethod( env, s_ActivityObject, mid, jpath );
	(*env)->DeleteLocalRef( env, jpath );
	(*env)->DeleteLocalRef( env, cls );
}
