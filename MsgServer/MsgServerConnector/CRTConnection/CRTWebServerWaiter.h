#ifndef __MsgServerConnector_CRTWEBSERVERWAITER_H__
#define __MsgServerConnector_CRTWEBSERVERWAITER_H__
#include "RTTcp.h"
#include "CRTWebServerHttp.h"
#include "OSMutex.h"
#include "RTObserverConnection.h"

class CRTWebServerWaiter
	: public RTTcp
	, public CRTWebServerHttp
    , public RTObserverConnection
{
public:
	CRTWebServerWaiter(void);
	virtual ~CRTWebServerWaiter(void);

public:
    int SendDispatch(const std::string& id, const std::string& msg);
public:
	//* For RCTcp
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen) {}

public:
	//* For RTConnHttp
    virtual void OnOtherLogin(const char* pContent);
	virtual void OnResponse(const char*pData, int nLen);

public:
    //* For RTObserverConnection
    virtual void ConnectionDisconnected();
private:
	char			*m_pBuffer;
	int				m_nBufLen;
	int				m_nBufOffset;
    std::string     m_connectorId;
    std::string     m_userId;
    std::string     m_token;
    std::string     m_nname;
    OSMutex         m_mutex;
};

#endif	// __MsgServerConnector_CRTWEBSERVERWAITER_H__
