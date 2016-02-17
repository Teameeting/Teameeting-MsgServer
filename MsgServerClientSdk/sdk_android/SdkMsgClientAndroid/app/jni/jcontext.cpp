#include "jni_helpers.h"
#include "jcontext.h"

#include "helpers_android.h"
#include "jni_utils.h"
#include "jni_helpers.h"

JavaVM* JContext::g_vm = NULL;

void NativeContextRegister(JavaVM* vm, JNIEnv* jni, jobject context)
{
	CHECK(vm, "Trying to register NULL vm");
	CHECK(!JContext::g_vm, "Trying to re-register vm");
	JContext::g_vm = vm;

	jni_set_java_context((void*)vm);
}

void NativeContextUnregister()
{
	CHECK(JContext::g_vm, "Clearing vm without it being set");
	JContext::g_vm = NULL;
	jni_unset_java_context();
}
