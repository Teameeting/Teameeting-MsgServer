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

void JMClientApp::OnLogin(int code)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnLogin callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnLogin", "(I)V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code);
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

void JMClientApp::OnGetMsg(const std::string& strFrom, const std::string& strMsg)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnGetMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnGetMsg", "(Ljava/lang/String;Ljava/lang/String;)V");
		// Callback with params
		JavaString jfrom(strFrom);
		JavaString jmsg(strMsg);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, jfrom.Get(), jmsg.Get());
	}
}

void JMClientApp::OnLogout(int code)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnLogout callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnLogout", "(I)V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code);
	}
}
