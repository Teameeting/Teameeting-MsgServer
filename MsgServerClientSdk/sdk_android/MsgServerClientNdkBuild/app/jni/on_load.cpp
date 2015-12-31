/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <jni.h>
#include "jni_helpers.h"

// Macro for native functions that can be found by way of jni-auto discovery.
// Note extern "C" is needed for "discovery" of native methods to work.
////#define JOWW(rettype, name)                                             \
////  extern "C" rettype JNIEXPORT JNICALL Java_org_dync_jni_##name
#define JOWW(rettype, name)                                             \
  extern "C" rettype JNIEXPORT JNICALL Java_com_dync_teameeting_msgserverclient_jni_##name

static JavaVM* g_vm = NULL;

extern void NativeContextRegister(JavaVM* vm, JNIEnv* jni, jobject context);
extern void NativeContextUnregister();

extern "C" jint JNIEXPORT JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
  // Only called once.
  CHECK(!g_vm, "OnLoad called more than once");
  g_vm = vm;
  return JNI_VERSION_1_4;
}

JOWW(void, NativeContextRegistry_register)(
    JNIEnv* jni, jclass, jobject context)
{
	NativeContextRegister(g_vm, jni, context);
}

JOWW(void, NativeContextRegistry_unRegister)(
    JNIEnv* jni, jclass)
{
	NativeContextUnregister();
}
