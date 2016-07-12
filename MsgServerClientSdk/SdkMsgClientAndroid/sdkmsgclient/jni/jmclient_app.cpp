#include "jmclient_app.h"
#include "jni_helpers.h"
#include "jcontext.h"
#include "helpers_android.h"
#include "java_string.h"

JMClientApp::JMClientApp(jobject javaObj)
: m_jJavaObj(NULL)
, m_jClass(NULL)
{
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

void JMClientApp::OnSndMsg(int code, const std::string& msgid)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnSndMsg", "(I;Ljava/lang/String;)V");
		// Callback with params
		JavaString jmsg(msgid);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code, jmsg.Get());
	}
}

void JMClientApp::OnCmdCallback(int code, int cmd, const std::string& groupid, const MSCbData& data)
{

}

void JMClientApp::OnRecvMsg(int64 seqn, const std::string& msg)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnRecvMsg", "(J;Ljava/lang/String;)V");
		// Callback with params
		JavaString jmsg(msg);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, seqn, jmsg.Get());
	}
}

void JMClientApp::OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnRecvGroupMsg", "(J;Ljava/lang/String;Ljava/lang/String;)V");
		// Callback with params
		JavaString jseqnid(seqnid);
		JavaString jmsg(msg);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, seqn, jseqnid.Get(), jmsg.Get());
	}
}

void JMClientApp::OnSyncSeqn(int64 seqn, int role)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnSyncSeqn", "(J;I;)V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, seqn, role);
	}
}

void JMClientApp::OnSyncGroupSeqn(const std::string& groupid, int64 seqn)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnSyncGroupSeqn", "(Ljava/lang/String;J;)V");
		// Callback with params
		JavaString jmsg(groupid);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, jmsg.Get(), seqn);
	}
}

void JMClientApp::OnGroupNotify(int code, const std::string& seqnid)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnGroupNotify", "(I;Ljava/lang/String;)V");
		// Callback with params
		JavaString jmsg(seqnid);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code, jmsg.Get());
	}
}

void JMClientApp::OnNotifySeqn(int code, const std::string& seqnid)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnNotifySeqn", "(I;Ljava/lang/String)V");
		// Callback with params
		JavaString jmsg(seqnid);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code, jmsg.Get());
	}
}

void JMClientApp::OnNotifyData(int code, const std::string& seqnid)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSndMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnNotifyData", "(I;Ljava/lang/String;)V");
		// Callback with params
		JavaString jmsg(seqnid);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code, jmsg.Get());
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

void JMClientApp::OnMsgServerConnecting()
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnLogin callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnMsgServerConnecting", "()V");
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
