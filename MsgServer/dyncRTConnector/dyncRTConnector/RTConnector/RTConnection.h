#ifndef __RT_CONNECTION_H__
#define __RT_CONNECTION_H__
#include "RTTcp.h"
#include "RTConnHttp.h"
#include "OSMutex.h"
#include "RTObserverConnection.h"

class RTConnection
	: public RTTcp
	, public RTConnHttp
    , public RTObserverConnection
{
public:
	RTConnection(void);
	virtual ~RTConnection(void);

	static std::string		gStrAddr;
	static unsigned short	gUsPort;
public:
    int SendDispatch(const std::string& id, const std::string& msg);
public:
	//* For RCTcp
	virtual void OnRecvData(const char*pData, int nLen);
	virtual void OnLcsEvent();
	virtual void OnPeerEvent(){Assert(false);};
	virtual void OnTickEvent(){};

public:
	//* For RTConnHttp
    virtual void OnLogin(const char* pUserid, const char* pPass);
    virtual void OnSndMsg(const char* pUserid, int mType, const char* pData, int dLen);
    virtual void OnGetMsg(const char* pUserid, int mType);
    virtual void OnLogout(const char* pUserid);
	virtual void OnResponse(const char*pData, int nLen);

public:
    //* For RTObserverConnection
    virtual void ConnectionDisconnected();
private:
    int GenericTransSeq();
private:
	char			*m_pBuffer;
	int				m_nBufLen;
	int				m_nBufOffset;
    std::string     m_connectorId;
    std::string     m_userId;
    std::string     m_token;
    OSMutex         m_mutex;
};

#endif	// __RT_CONNECTION_H__
