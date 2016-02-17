#include "jmclient_app.h"
#include <stdio.h>
#include <stdlib.h>
#include "jni_helpers.h"
#include "jcontext.h"

#include "helpers_android.h"
#include "java_string.h"
#include "jni_helpers.h"
#include "org_dync_teameeting_sdkmsgclient_jni_JMClientApp.h"

// Macro for native functions that can be found by way of jni-auto discovery.
// Note extern "C" is needed for "discovery" of native methods to work.
////#define JOWW(rettype, name)                                             \
////  extern "C" rettype JNIEXPORT JNICALL Java_org_dync_jni_##name
#define JOWW(rettype, name)                                             \
  extern "C" rettype JNIEXPORT JNICALL Java_org_dync_teameeting_sdkmsgclient_jni_##name


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
	return jApp->MSStatus();
}

JOWW(int, JMClientApp_Init)(JNIEnv* jni, jobject j_app, jstring strUid, jstring strToken, jstring strNname, jstring strSvrAddr, jint nSvrPort)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUid(strUid);
	JavaString jstrToken(strToken);
	JavaString jstrNname(strNname);
	JavaString jstrSvrAddr(strSvrAddr);

	return jApp->Init((*jApp), jstrUid.ToString8().c_str(), jstrToken.ToString8().c_str(), jstrNname.ToString8().c_str(), jstrSvrAddr.ToString8().c_str(), nSvrPort);
}

JOWW(int, JMClientApp_SndMsg)(JNIEnv* jni, jobject j_app, jstring strRoomid, jstring strRname, jstring strMsg)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrRoomid(strRoomid);
	JavaString jstrRname(strRname);
	JavaString jstrMsg(strMsg);
	return jApp->SndMsg(jstrRoomid.ToString8().c_str(), jstrRname.ToString8().c_str(), jstrMsg.ToString8().c_str());
}

JOWW(int, JMClientApp_GetMsg)(JNIEnv* jni, jobject j_app, jint icmd)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->GetMsg((GETCMD)icmd);
}

JOWW(int, JMClientApp_OptRoom)(JNIEnv* jni, jobject j_app, jint icmd, jstring strRoomid, jstring strRname, jstring strRemain)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrRoomid(strRoomid);
	JavaString jstrRname(strRname);
	JavaString jstrRemain(strRemain);
	return jApp->OptRoom((MEETCMD)icmd, jstrRoomid.ToString8().c_str(), jstrRname.ToString8().c_str(), jstrRemain.ToString8().c_str());
}

JOWW(int, JMClientApp_SndMsgTo)(JNIEnv* jni, jobject j_app, jstring strRoomid, jstring strRname, jstring strMsg, jobjectArray arrUser)
{
	return 0;
}

JOWW(int, JMClientApp_NotifyMsg)(JNIEnv* jni, jobject j_app, jstring strRoomid, jstring strRname, jint itags, jstring strMsg)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrRoomid(strRoomid);
	JavaString jstrRname(strRname);
	JavaString jstrMsg(strMsg);
	return jApp->NotifyMsg(jstrRoomid.ToString8().c_str(), jstrRname.ToString8().c_str(), (SENDTAGS)itags, jstrMsg.ToString8().c_str());
}

JOWW(void, JMClientApp_SetNickName)(JNIEnv* jni, jobject j_app, jstring strNname)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrNname(strNname);
	jApp->SetNickName(jstrNname.ToString8().c_str());
}

JOWW(int, JMClientApp_Unin)(JNIEnv* jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->Unin();
}