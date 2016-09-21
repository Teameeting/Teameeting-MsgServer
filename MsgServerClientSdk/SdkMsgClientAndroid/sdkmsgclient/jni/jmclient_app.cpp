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

		jclass jcbDataClsTmp = ats.env()->FindClass("org/dync/teameeting/sdkmsgclient/jni/JMSCbData");
		m_jCbDataCls = reinterpret_cast<jclass> (ats.env()->NewGlobalRef(jcbDataClsTmp));
		ats.env()->DeleteLocalRef(jcbDataClsTmp);

		jclass jstrClsTmp = ats.env()->FindClass("java/lang/String");
		m_jStrCls = reinterpret_cast<jclass> (ats.env()->NewGlobalRef(jstrClsTmp));
		ats.env()->DeleteLocalRef(jstrClsTmp);
	}
}

JMClientApp::~JMClientApp(void)
{
	Close();
}

void JMClientApp::Close()
{
	if(m_jStrCls)
	{
		AttachThreadScoped ats(JContext::g_vm);
		ats.env()->DeleteGlobalRef(m_jStrCls);
		m_jStrCls = NULL;
	}
	if(m_jCbDataCls)
	{
		AttachThreadScoped ats(JContext::g_vm);
		ats.env()->DeleteGlobalRef(m_jCbDataCls);
		m_jCbDataCls = NULL;
	}
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
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnSndMsg", "(ILjava/lang/String;)V");
		// Callback with params
		JavaString jmsg(msgid);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code, jmsg.Get());
	}
}

void JMClientApp::OnCmdCallback(int code, int cmd, const std::string& groupid, const MSCbData& data)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnCmdCallback callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnCmdCallback", "(IILjava/lang/String;Lorg/dync/teameeting/sdkmsgclient/jni/JMSCbData;)V");
		// Callback with params
		JavaString jgroupid(groupid);
		JavaString jdatadata(data.data);

		jmethodID cbdata_construct = jni->GetMethodID(m_jCbDataCls, "<init>", "()V");

		jobject jcbdata = jni->NewObject(m_jCbDataCls, cbdata_construct);
		jfieldID typeID = jni->GetFieldID(m_jCbDataCls, "type", "I");
		jfieldID dataID = jni->GetFieldID(m_jCbDataCls, "data", "Ljava/lang/String;");
		jfieldID seqnID = jni->GetFieldID(m_jCbDataCls, "seqn", "J");
		jfieldID resultID = jni->GetFieldID(m_jCbDataCls, "result", "I");

		jni->SetIntField(jcbdata, typeID, data.type);
		jni->SetObjectField(jcbdata, dataID, jdatadata.Get());
		jni->SetLongField(jcbdata, seqnID, data.seqn);
		jni->SetIntField(jcbdata, resultID, data.result);

		LGI("JMClientApp::OnCmdCallback code:%d, cmd:%d, groupid:%s, data.type:%d, data.seqn:%lld, data.result:%d, data.data:%s\n", code, cmd, groupid.c_str(), data.type, data.seqn, data.result, data.data.c_str());

		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code, cmd, jgroupid.Get(), jcbdata);

	}
}

void JMClientApp::OnRecvMsg(int64 seqn, const std::string& msg)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnRecvMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnRecvMsg", "(J[B)V");

		// Callback with params
		LGI("--->JMClientApp::OnRecvMsg msg.length:%d\n", msg.length());
		pms::Entity entity;
		entity.ParseFromString(msg);
		const std::string jss = entity.SerializeAsString();

		LGI("JMClientApp::OnRecvMsg entity.usr_from:%s, rom_id:%s, msg_cont.size:%d, jss.size:%d\n"\
            , entity.usr_from().c_str()\
            , entity.rom_id().c_str()\
            , entity.msg_cont().length()\
			, jss.length());
		jbyteArray bytes = jni->NewByteArray((jsize)jss.length());
		jni->SetByteArrayRegion(bytes,0, (jsize)jss.length(), (jbyte*)jss.c_str());
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, seqn, bytes);
	}
}

void JMClientApp::OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnRecvGroupMsg callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnRecvGroupMsg", "(JLjava/lang/String;[B)V");
		// Callback with params
		JavaString jseqnid(seqnid);
		LGI("--->JMClientApp::OnRecvGroupMsg msg.length:%d\n", msg.length());
		pms::Entity entity;
		entity.ParseFromString(msg);
		const std::string jss = entity.SerializeAsString();

		LGI("JMClientApp::OnRecvGroupMsg entity.usr_from:%s, rom_id:%s, msg_cont.size:%d, jss.size:%d\n"\
            , entity.usr_from().c_str()\
            , entity.rom_id().c_str()\
            , entity.msg_cont().length()\
			, jss.length());
		jbyteArray bytes = jni->NewByteArray((jsize)jss.length());
		jni->SetByteArrayRegion(bytes,0, (jsize)jss.length(), (jbyte*)jss.c_str());
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, seqn, jseqnid.Get(), bytes);
	}
}

void JMClientApp::OnSyncSeqn(int64 seqn, int role)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSyncSeqn callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnSyncSeqn", "(JI)V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, seqn, role);
	}
}

void JMClientApp::OnSyncGroupSeqn(const std::string& groupid, int64 seqn)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnSyncGroupSeqn callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnSyncGroupSeqn", "(Ljava/lang/String;J)V");
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
		// Get OnGroupNotify callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnGroupNotify", "(ILjava/lang/String;)V");
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
		// Get OnNotifySeqn callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnNotifySeqn", "(ILjava/lang/String;)V");
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
		// Get OnNotifyData callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnNotifyData", "(ILjava/lang/String;)V");
		// Callback with params
		JavaString jmsg(seqnid);
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code, jmsg.Get());
	}
}

void JMClientApp::OnNotifyOtherLogin(int code)
{
	AttachThreadScoped ats(JContext::g_vm);
	JNIEnv* jni = ats.env();
	{
		// Get OnNotifyOtherLogin callback interface method id
		jmethodID j_callJavaMId = JGetMethodID(jni, m_jClass, "OnNotifyOtherLogin", "(I)V");
		// Callback with params
		jni->CallVoidMethod(m_jJavaObj, j_callJavaMId, code);
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
