#ifndef __JMCLIENT_APP_H__
#define __JMCLIENT_APP_H__
#include <jni.h>
#include "XMsgCallback.h"
#include "XMsgClient.h"

class JMClientApp : public XMsgClient, public XMsgCallback
{
public:
	JMClientApp(jobject javaObj);
	virtual ~JMClientApp(void);

	void Close();
public:
	//* For XMsgCallback
	virtual void OnSndMsg(const std::string& msg);
	virtual void OnGetMsg(const std::string& msg);

	virtual void OnMsgServerConnected();
	virtual void OnMsgServerDisconnect();
	virtual void OnMsgServerConnectionFailure();
	virtual void OnMsgServerState(MSState state);

private:
	jobject     m_jJavaObj;
	jclass		m_jClass;
};


#endif	// __JMCLIENT_APP_H__