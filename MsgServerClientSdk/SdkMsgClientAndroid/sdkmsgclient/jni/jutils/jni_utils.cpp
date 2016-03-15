#include "jni_utils.h"
#include <jni.h>
#include "jni_helpers.h"
#include "helpers_android.h"

JavaVM* g_vm = NULL;
int	g_sysVersion = 0;

void jni_set_java_context(void* jvm)
{
	//CHECK(!g_vm, "Trying to re-register jutils vm");
	g_vm = (JavaVM*)jvm;

	{
		AttachThreadScoped ats(g_vm);
		JNIEnv* env = ats.env();
		g_sysVersion = GetAndroidVersion(env);
	}
}

void jni_unset_java_context()
{
	g_vm = NULL;
}
