/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "jni_helpers.h"

#include <limits>
#include "java_string.h"

jmethodID JGetMethodID(JNIEnv* jni, jclass c, const char* name,
                      const char* signature) {
  jmethodID m = jni->GetMethodID(c, name, signature);
  char buffer[256] = {0};
  sprintf(buffer, "Error during GetMethodID: %s%s", name, signature);
  CHECK_EXCEPTION(jni, buffer);
  return m;
}

int GetNativeFd(JNIEnv* env, jobject fdesc) {
    jclass clazz;
    jfieldID fid;

    /* get the fd from the FileDescriptor */
    if (!(clazz = env->GetObjectClass(fdesc)) ||
            !(fid = env->GetFieldID(clazz,"descriptor","I"))) return -1;

    /* return the descriptor */
    return env->GetIntField(fdesc,fid);
}

jobject GetSurface(JNIEnv* env, jobject jsurfaceView)
{	
	jobject jsurface = NULL;
	jclass cls_surfaceView = env->GetObjectClass(jsurfaceView);
    jmethodID getSurfaceHolder = env->GetMethodID(cls_surfaceView, "getHolder", "()Landroid/view/SurfaceHolder;");
	/*Java code: SurfaceView.getHolder();*/
    jobject jsurfaceHolder = env->CallObjectMethod(jsurfaceView, getSurfaceHolder);
	
	if(NULL != jsurfaceHolder)
	{
		//jclass cls_surfaceHolder = env->FindClass("android/view/SurfaceHolder");
		jclass cls_surfaceHolder = env->GetObjectClass(jsurfaceHolder);
		jmethodID getSurface = env->GetMethodID(cls_surfaceHolder, "getSurface", "()Landroid/view/Surface;");
		/*Java code: surfaceHolder.getSurface();*/
		jsurface = env->CallObjectMethod(jsurfaceHolder, getSurface);
		
		env->DeleteLocalRef(cls_surfaceHolder);
	}
	else
	{
		LGE("Find getHolder error!");
	}
	env->DeleteLocalRef(cls_surfaceView);
	
	return jsurface;
}

int GetSurfaceId(JNIEnv* env, jobject jsurfaceView)
{
	int surid = 0;
	
	jobject jsurface = GetSurface(env, jsurfaceView);
	if(NULL != jsurface)
	{
		// Get surface id.
		jfieldID f_Surface_mSurface;
		//jclass cls_surface = env->FindClass("android/view/Surface");
		jclass cls_surface = env->GetObjectClass(jsurface);
		f_Surface_mSurface = env->GetFieldID(cls_surface, "mSurface", "I");
		if (f_Surface_mSurface == 0)
		{
			jthrowable e = env->ExceptionOccurred();
			if (e)
			{
				env->DeleteLocalRef(e);
				env->ExceptionClear();
			}
			f_Surface_mSurface = env->GetFieldID(cls_surface, "mNativeSurface", "I");
			if(f_Surface_mSurface)
			{
				jthrowable e = env->ExceptionOccurred();
				if (e)
				{
					env->DeleteLocalRef(e);
					env->ExceptionClear();
				}
			}
		}
		if(f_Surface_mSurface > 0)
			surid = env->GetIntField(jsurface, f_Surface_mSurface);
		else
		{
			LGE("Find mSurface error!");
		}
		env->DeleteLocalRef(cls_surface);
	}
	else
	{
		LGE("Find getSurface error!");
	}
	
	return surid;
}

jlong jlongFromPointer(void* ptr) {
  CHECK(sizeof(intptr_t) <= sizeof(jlong), "Time to rethink the use of jlongs");
  // Going through intptr_t to be obvious about the definedness of the
  // conversion from pointer to integral type.  intptr_t to jlong is a standard
  // widening by the COMPILE_ASSERT above.
  jlong ret = reinterpret_cast<intptr_t>(ptr);
  CHECK(reinterpret_cast<void*>(ret) == ptr,
        "jlong does not convert back to pointer");
  return ret;
}

// Given a (UTF-16) jstring return a new UTF-8 native string.
std::string JavaToStdString(JNIEnv* jenv, const jstring& j_string) {
  JavaString jstr(j_string);
  return jstr.ToString8();
}

// Get android device id.
int GetAndroidDeviceId(JNIEnv *jenv, jobject jcontext, char* device_id)
{
    static char _device_id[64];
    const char *android_id, *imei, *serial;
    jstring     jandroid_id, jimei, jserial;

    if (!jenv || !jcontext) {
    	memset(device_id, 0, 64);
       return -1;
    }

    /* String androidId = android.provider.Settings.Secure.getString(getApplicationContext().getContentResolver(), android.provider.Settings.Secure.ANDROID_ID); */
    jclass cls_context = jenv->FindClass("android/content/Context");
    jmethodID getMethod = jenv->GetMethodID(cls_context, "getContentResolver", "()Landroid/content/ContentResolver;");
    jobject resolver = jenv->CallObjectMethod(jcontext, getMethod);
    jclass cls_secure = jenv->FindClass("android/provider/Settings$Secure");
    jmethodID getStringMethod = jenv->GetStaticMethodID(cls_secure, "getString", "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");
    jfieldID ANDROID_ID = jenv->GetStaticFieldID(cls_secure,
                "ANDROID_ID", "Ljava/lang/String;");
    jstring strAndroidId = (jstring)(jenv->GetStaticObjectField(cls_secure, ANDROID_ID));
    jandroid_id = (jstring)(jenv->CallStaticObjectMethod(cls_secure, getStringMethod, resolver, strAndroidId));

    /* String imei = ((android.telephony.TelephonyManager) getApplicationContext().getSystemService(android.content.Context.TELEPHONY_SERVICE)).getDeviceId(); */
    jobject telephonymanager = jenv->CallObjectMethod(jcontext, jenv->GetMethodID(jenv->GetObjectClass(jcontext), "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;"), jenv->NewStringUTF("phone"));
    jmethodID getDeviceId = jenv->GetMethodID(jenv->FindClass("android/telephony/TelephonyManager"), "getDeviceId", "()Ljava/lang/String;");
    jimei = (jstring)jenv->CallObjectMethod(telephonymanager, getDeviceId);

    /* String serial = android.os.Build.SERIAL; */
    jclass cls_build = jenv->FindClass("android/os/Build");
    jfieldID SERIAL = jenv->GetStaticFieldID(cls_build, "SERIAL", "Ljava/lang/String;");
    jserial = (jstring)jenv->GetStaticObjectField(cls_build, SERIAL);

    android_id = jandroid_id ? jenv->GetStringUTFChars( jandroid_id, NULL) : NULL;
    imei       = jimei ? jenv->GetStringUTFChars( jimei, NULL) : NULL;
    serial     = jserial ? jenv->GetStringUTFChars( jserial, NULL) : NULL;

    //LD("android_id:%s imei:%s serial:%s", android_id, imei, serial);
    // #include <sys/system_properties.h>
    // char serial[32];
    // __system_property_get("ro.serialno",serial);

    if (android_id && strcmp(android_id, "") && strcmp(android_id, "9774d56d682e549c")) {
        strcpy(_device_id, android_id);
    } else if (imei && strcmp(imei, "")) {
        strcpy(_device_id, imei);
    } else if  (serial && strcmp(serial, "")) {
        strcpy(_device_id, serial);
    } else {
        strcpy(_device_id, "");
    }

    if (strlen(_device_id) <= 10) {
        strcpy(_device_id, "");
    }

	if(android_id)
		jenv->ReleaseStringUTFChars( jandroid_id, android_id);
	if(imei)
		jenv->ReleaseStringUTFChars( jimei, imei);
	if(serial)
		jenv->ReleaseStringUTFChars( jserial, serial);

    if (device_id) {
        strcpy(device_id, _device_id);
    }

    return 0;
}

// Get android version
int GetAndroidVersion(JNIEnv *jenv)
{
	//android.os.Build.VERSION
	/* int sdk_version = android.os.Build.VERSION.SDK_INT; */
    jclass cls_VERSION = jenv->FindClass("android/os/Build$VERSION");
    jfieldID SDK_INT = jenv->GetStaticFieldID(cls_VERSION, "SDK_INT", "I");
    jint sdk_version = jenv->GetStaticIntField(cls_VERSION, SDK_INT);

   return sdk_version;
}

ClassReferenceHolder::ClassReferenceHolder(JNIEnv* jni, const char** classes,
                                           int size) {
  for (int i = 0; i < size; ++i) {
    LoadClass(jni, classes[i]);
  }
}
ClassReferenceHolder::~ClassReferenceHolder() {
  CHECK(classes_.empty(), "Must call FreeReferences() before dtor!");
}

void ClassReferenceHolder::FreeReferences(JNIEnv* jni) {
  for (std::map<std::string, jclass>::const_iterator it = classes_.begin();
       it != classes_.end(); ++it) {
    jni->DeleteGlobalRef(it->second);
  }
  classes_.clear();
}

jclass ClassReferenceHolder::GetClass(const std::string& name) {
  std::map<std::string, jclass>::iterator it = classes_.find(name);
  CHECK(it != classes_.end(), "Could not find class");
  return it->second;
}

void ClassReferenceHolder::LoadClass(JNIEnv* jni, const std::string& name) {
  jclass localRef = jni->FindClass(name.c_str());
  CHECK_EXCEPTION(jni, "Could not load class");
  CHECK(localRef, name.c_str());
  jclass globalRef = reinterpret_cast<jclass>(jni->NewGlobalRef(localRef));
  CHECK_EXCEPTION(jni, "error during NewGlobalRef");
  CHECK(globalRef, name.c_str());
  bool inserted = classes_.insert(std::make_pair(name, globalRef)).second;
  CHECK(inserted, "Duplicate class name");
}
