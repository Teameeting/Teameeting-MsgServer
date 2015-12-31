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
	virtual void OnLogin(int code);
    virtual void OnSndMsg(const std::string& msg);
    virtual void OnGetMsg(const std::string& from, const std::string& msg);
    virtual void OnLogout(int code);

private:
	jobject     m_jJavaObj;
	jclass		m_jClass;
};


#endif	// __JMCLIENT_APP_H__