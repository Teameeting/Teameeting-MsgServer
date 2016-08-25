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

JOWW(jlong, JMClientApp_Create)(JNIEnv *jni, jclass j_app, jobject j_obj)
{
	//JavaString jstrLogPath(jlogPath);
	//L_Init(jlogLevel, jstrLogPath.ToString8().c_str());

	JMClientApp* jApp = new JMClientApp(j_obj);
	return jlongFromPointer(jApp);
}


JOWW(jint, JMClientApp_Init)(JNIEnv *jni, jobject j_app, jstring strUid, jstring strToken, jstring strNname, jint devType, jint push, jint module)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUid(strUid);
	JavaString jstrToken(strToken);
	JavaString jstrNname(strNname);
	return jApp->Init(jstrUid.ToString8().c_str(), jstrToken.ToString8().c_str(), jstrNname.ToString8().c_str(), devType, push, module);
}


JOWW(jint, JMClientApp_Unin)(JNIEnv *jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->Unin();
}


JOWW(jint, JMClientApp_RegisterMsgCb)(JNIEnv *jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->RegisterMsgCb(jApp);
}


JOWW(jint, JMClientApp_UnRegisterMsgCb)(JNIEnv *jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->UnRegisterMsgCb(jApp);
}


JOWW(jint, JMClientApp_ConnToServer)(JNIEnv *jni, jobject j_app, jstring strServer, jint port)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrServer(strServer);
	return jApp->ConnToServer(jstrServer.ToString8().c_str(), port);
}


JOWW(jint, JMClientApp_AddGroup)(JNIEnv *jni, jobject j_app, jstring strGroupId)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrGroupId(strGroupId);
	return jApp->AddGroup(jstrGroupId.ToString8().c_str());
}


JOWW(jint, JMClientApp_RmvGroup)(JNIEnv *jni, jobject j_app, jstring strGroupId)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrGroupId(strGroupId);
	return jApp->RmvGroup(jstrGroupId.ToString8().c_str());
}


JOWW(jstring, JMClientApp_SndMsg)(JNIEnv *jni, jobject j_app, jstring strGroupId, jstring strGroupName, jstring strMsg, jint tag, jint type, jint module, jint flag, jint push)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	std::string soutMsgId;
	JavaString jstrGroupId(strGroupId);
	JavaString jstrGroupName(strGroupName);
	JavaString jstrMsg(strMsg);
	jApp->SndMsg(soutMsgId, jstrGroupId.ToString8().c_str(), jstrGroupName.ToString8().c_str(), jstrMsg.ToString8().c_str(), tag, type, module, flag, push);
	return jni->NewStringUTF(soutMsgId.c_str());
}


JOWW(jstring, JMClientApp_SndMsgTo)(JNIEnv *jni, jobject j_app, jstring strGroupId, jstring strGroupName, jstring strMsg, jint tag, jint type, jint module, jint flag, jint push, jobjectArray users, jint size)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	std::string soutMsgId;
	JavaString jstrGroupId(strGroupId);
	JavaString jstrGroupName(strGroupName);
	JavaString jstrMsg(strMsg);
	std::vector<std::string> vusers;
	for (int i=0;i<size;i++) {
		jstring js = (jstring)jni->GetObjectArrayElement(users, i);
		vusers.push_back(jni->GetStringUTFChars(js, nullptr));
	}
	jApp->SndMsgTo(soutMsgId, jstrGroupId.ToString8().c_str(), jstrGroupName.ToString8().c_str(), jstrMsg.ToString8().c_str(), tag, type, module, flag, push, vusers);
	return jni->NewStringUTF(soutMsgId.c_str());
}


JOWW(jint, JMClientApp_FetchSeqn)(JNIEnv *jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->FetchSeqn();
}


JOWW(jint, JMClientApp_SyncSeqn)(JNIEnv *jni, jobject j_app, jlong seqn, jint role)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->SyncSeqn(seqn, role);
}


JOWW(jint, JMClientApp_SyncData)(JNIEnv *jni, jobject j_app, jlong seqn)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->SyncData(seqn);
}


JOWW(jint, JMClientApp_FetchGroupSeqn)(JNIEnv *jni, jobject j_app, jstring strGroupId)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrGroupId(strGroupId);
	return jApp->FetchGroupSeqn(jstrGroupId.ToString8().c_str());
}


JOWW(jint, JMClientApp_SyncGroupSeqn)(JNIEnv *jni, jobject j_app, jstring strGroupId, jlong seqn, jint role)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrGroupId(strGroupId);
	return jApp->SyncGroupSeqn(jstrGroupId.ToString8().c_str(), seqn, role);
}


JOWW(jint, JMClientApp_SyncGroupData)(JNIEnv *jni, jobject j_app, jstring strGroupId, jlong seqn)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrGroupId(strGroupId);
	return jApp->SyncGroupData(jstrGroupId.ToString8().c_str(), seqn);
}


JOWW(jint, JMClientApp_ConnStatus)(JNIEnv * jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	return jApp->MSStatus();
}


JOWW(void, JMClientApp_SetUserid)(JNIEnv *jni, jobject j_app, jstring strUserid)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUserid(strUserid);
	jApp->SetUserId(jstrUserid.ToString8().c_str());
}


JOWW(void, JMClientApp_SetToken)(JNIEnv *jni, jobject j_app, jstring strToken)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrToken(strToken);
	jApp->SetToken(jstrToken.ToString8().c_str());
}

JOWW(void, JMClientApp_SetNickName)(JNIEnv *jni, jobject j_app, jstring strNname)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrNname(strNname);
	jApp->SetNickName(jstrNname.ToString8().c_str());
}


JOWW(void, JMClientApp_SetUIconUrl)(JNIEnv *jni, jobject j_app, jstring strUIcon)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrUIcon(strUIcon);
	jApp->SetUIconUrl(jstrUIcon.ToString8().c_str());
}

JOWW(void, JMClientApp_EnablePush)(JNIEnv *jni, jobject j_app, jint push)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	jApp->SetEnablePush(push);
}

JOWW(void, JMClientApp_UpdateSetting)(JNIEnv *jni, jobject j_app, jlong setType, jstring jsonSetting)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jjsonSetting(jsonSetting);
	jApp->UpdateSetting(setType, jjsonSetting.ToString8().c_str());
}

JOWW(void, JMClientApp_InitUserSeqns)(JNIEnv *jni, jobject j_app, jstring strSeqnid, jlong seqn)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrSeqnid(strSeqnid);
	jApp->InitUserSeqns(jstrSeqnid.ToString8().c_str(), seqn);
}

JOWW(void, JMClientApp_UpdateUserSeqns)(JNIEnv *jni, jobject j_app, jstring strSeqnid, jlong seqn)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	JavaString jstrSeqnid(strSeqnid);
	jApp->UpdateUserSeqns(jstrSeqnid.ToString8().c_str(), seqn);
}

JOWW(void, JMClientApp_Destroy)(JNIEnv *jni, jobject j_app)
{
	JMClientApp* jApp = GetJApp(jni, j_app);
	jApp->Close();
	delete jApp;
}
