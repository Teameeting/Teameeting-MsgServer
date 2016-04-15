#include "jmclient_app.h"
#include "jni_helpers.h"
#include "jcontext.h"
#include "helpers_android.h"
#include "java_string.h"

JMClientApp::JMClientApp(jobject javaObj)
: m_jJavaObj(NULL)
, m_jClass(NULL)
{
	if(javaObj)
	{
		AttachThreadScoped ats(JContext::g_vm);
		m_jJavaObj = ats.env()->NewGlobalRef(javaObj);
		m_jClass = reinterpret_cast<jclass> (ats.env()->NewGlobalRef(ats.env()->GetObjectClass(m_jJavaObj)));
	}
}

JMClientApp::~JMClientApp(void)
{
	Close();
}

void JMClientApp::Close()
{
	if(m_jJavaObj)
	{
		AttachThreadScoped ats(JContext::g_vm);
		ats.env()->DeleteGlobalRef(m_jClass);
		m_jClass = NULL;
		ats.env()->DeleteGlobalRef(m_jJavaObj);
		m_jJavaObj = NULL;
	}
}

void JMClientApp::OnSndMsg(const std::string& strMsg)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnSndMsg", "(Ljava/lang/String;)V");
		// Callback with params
		JavaString jmsg(strMsg);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, jmsg.Get());
	}
}

void JMClientApp::OnGetMsg(const std::string& strMsg)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnGetMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnGetMsg", "(Ljava/lang/String;)V");
		// Callback with params
		JavaString jmsg(strMsg);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, jmsg.Get());
	}
}

void JMClientApp::OnMsgServerConnected()
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnLogin callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnMsgServerConnected", "()V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId);
	}
}

void JMClientApp::OnMsgServerDisconnect()
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnLogin callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnMsgServerDisconnect", "()V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId);
	}
}

void JMClientApp::OnMsgServerConnectionFailure()
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnLogin callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnMsgServerConnectionFailure", "()V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId);
	}
}

void JMClientApp::OnMsgServerState(MSState state)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnLogin callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnMsgServerState",
											   "(I)V");
		// Callback with params
		jint istate = (jint)state;
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, istate);
	}
}
