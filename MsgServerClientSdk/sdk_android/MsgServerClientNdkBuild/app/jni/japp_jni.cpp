#include "jmclient_app.h"
#include <stdio.h>
#include <stdlib.h>
#include "jni_helpers.h"
#include "jcontext.h"

#include "helpers_android.h"
#include "java_string.h"
#include "jni_helpers.h"
#include "com_dync_teameeting_msgserverclient_jni_JMClientApp.h"

// Macro for native functions that can be found by way of jni-auto discovery.
// Note extern "C" is needed for "discovery" of native methods to work.
////#define JOWW(rettype, name)                                             \
////  extern "C" rettype JNIEXPORT JNICALL Java_org_dync_jni_##name
#define JOWW(rettype, name)                                             \
  extern "C" rettype JNIEXPORT JNICALL Java_com_dync_teameeting_msgserverclient_jni_##name


//=================================================================
namespace {
JMClientApp* GetJApp(JNIEnv* jni, jobject j_app)
{
  jclass j_app_class = jni->GetObjectClass(j_app);
  jfieldID native_id =
      jni->GetFieldID(j_app_class, "fNativeAppId", "J");
  jlong j_p = jni->GetLongField(j_app, native_id);
  return reinterpret_cast<JMClientApp*>(j_p);
}
}

//=================================================================
//=================================================================
JOWW(jlong, JMClientApp_Create)(JNIEnv* jni, jclass, jobject j_obj)
{
	//JavaString jstrLogPath(jlogPath);
	//L_Init(jlogLevel, jstrLogPath.ToString8().c_str());

	JMClientApp* jApp = new JMClientApp(j_obj);
	return jlongFromPointer(jApp);
}

JOWW(void, JMClientApp_Destroy)(JNIEnv* jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	jApp->Close();
	delete jApp;
}

JOWW(jint, JMClientApp_ConnStatus)(JNIEnv* jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->Status();
}

JOWW(int, JMClientApp_Init)(JNIEnv* jni, jobject j_app, jstring strSvrAddr, jint nSvrPort)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrSvrAddr(strSvrAddr);

	return jApp->Init((*jApp), jstrSvrAddr.ToString8().c_str(), nSvrPort);
}

JOWW(int, JMClientApp_Login)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	return jApp->Login(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str());
}

JOWW(int, JMClientApp_SndMsg)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass, jstring strRoomid, jstring strMsg)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	JavaString jstrRoomid(strRoomid);
	JavaString jstrMsg(strMsg);
	return jApp->SndMsg(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str(), jstrRoomid.ToString8().c_str(), jstrMsg.ToString8().c_str());
}

JOWW(int, JMClientApp_GetMsg)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	return jApp->GetMsg(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str());
}

JOWW(int, JMClientApp_Logout)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	return jApp->Logout(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str());
}

JOWW(int, JMClientApp_CreateRoom)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass, jstring strRoomid)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	JavaString jstrRoomid(strRoomid);
	return jApp->CreateRoom(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str(), jstrRoomid.ToString8().c_str());
}

JOWW(int, JMClientApp_EnterRoom)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass, jstring strRoomid)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	JavaString jstrRoomid(strRoomid);
	return jApp->EnterRoom(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str(), jstrRoomid.ToString8().c_str());
}

JOWW(int, JMClientApp_LeaveRoom)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass, jstring strRoomid)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	JavaString jstrRoomid(strRoomid);
	return jApp->LeaveRoom(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str(), jstrRoomid.ToString8().c_str());
}

JOWW(int, JMClientApp_DestroyRoom)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass, jstring strRoomid)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	JavaString jstrRoomid(strRoomid);
	return jApp->DestroyRoom(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str(), jstrRoomid.ToString8().c_str());
}

JOWW(int, JMClientApp_SndMsgTo)(JNIEnv* jni, jobject j_app, jstring strUserid, jstring strPass, jstring strRoomid, jstring strMsg, jobjectArray arrUser)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	JavaString jstrPass(strPass);
	JavaString jstrRoomid(strRoomid);
	JavaString jstrMsg(strMsg);
	//jsize size = (jni)->GetArrayLength(arrUser);
	//for (int i = 0; i < size; ++i) {
	//	jstring s = (jstring)(jni)->GetObjectArrayElement(arrUser, i);
	//}
	//return jApp->SndMsgTo(jstrUserid.ToString8().c_str(), jstrPass.ToString8().c_str(), jstrRoomid.ToString8().c_str(), jstrMsg.ToString8().c_str());
	return 0;
}

JOWW(int, JMClientApp_Unin)(JNIEnv* jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->Unin();
}