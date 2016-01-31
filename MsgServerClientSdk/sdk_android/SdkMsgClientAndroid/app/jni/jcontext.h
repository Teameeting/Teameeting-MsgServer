#ifndef __J_CONTEXT_H__
#define __J_CONTEXT_H__
#include <jni.h>

void NativeContextRegister(JavaVM* vm, JNIEnv* jni, jobject context);
void NativeContextUnregister();

class JContext
{
public:
	JContext(void){};
	virtual ~JContext(void){};
public:
	static JavaVM* g_vm;
	static ClassReferenceHolder* g_class_reference_holder;
	static const char* g_classes[];
	static jclass GetClass(const char* name);
};

#endif
