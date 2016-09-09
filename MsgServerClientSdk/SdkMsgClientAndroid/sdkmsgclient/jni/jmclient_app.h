#ifndef __JMCLIENT_APP_H__
#define __JMCLIENT_APP_H__
#include <jni.h>
#include "core/XMsgCallback.h"
#include "core/XMsgClient.h"

class JMClientApp : public XMsgClient, public XMsgCallback
{
public:
	JMClientApp(jobject javaObj);
	virtual ~JMClientApp(void);

	void Close();
public:
	//* For XMsgCallback
	virtual void OnSndMsg(int code, const std::string& msgid);

	virtual void OnCmdCallback(int code, int cmd, const std::string& groupid, const MSCbData& data);
	virtual void OnRecvMsg(int64 seqn, const std::string& msg);
	virtual void OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg);

	virtual void OnSyncSeqn(int64 seqn, int role);
	virtual void OnSyncGroupSeqn(const std::string& groupid, int64 seqn);

	virtual void OnGroupNotify(int code, const std::string& seqnid);
	virtual void OnNotifySeqn(int code, const std::string& seqnid);
	virtual void OnNotifyData(int code, const std::string& seqnid);
	virtual void OnNotifyOtherLogin(int code);

public:
	virtual void OnMsgServerConnected();
	virtual void OnMsgServerConnecting();
	virtual void OnMsgServerDisconnect();
	virtual void OnMsgServerConnectionFailure();

private:
	jobject     m_jJavaObj;
	jclass		m_jClass;

	jclass 		m_jCbDataCls;

	jclass		m_jStrCls;
};


#endif	// __JMCLIENT_APP_H__