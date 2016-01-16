#ifndef __MsgServerMeeting_MRT_HTTP_SENDER_H__
#define __MsgServerMeeting_MRT_HTTP_SENDER_H__
#include "RTTcp.h"
#include "MRTConnHttp.h"
#include "OSMutex.h"
#include "RTMessage.h"
#include "MRTHttpCmd.h"

class MRTHttpSender
	: public RTTcp
	, public MRTConnHttp
{
public:
	MRTHttpSender(void);
    MRTHttpSender(HTTPCMD cmd, TRANSMSG& tmsg, MEETMSG& msg);
	virtual ~MRTHttpSender(void);

public:
    bool ConnHttpHost(const std::string& addr, const unsigned short port, const std::string& host);
    void SendRequest(const char* pData, int nLen);
    
public:
	//* For RCTcp
	virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnLcsEvent(){}
    virtual void OnPeerEvent(){}
	virtual void OnTickEvent(){}

public:
	//* For RTConnHttp
	virtual void OnResponse(const char*pData, int nLen);

private:
	char			*m_pBuffer;
	int				m_nBufLen;
	int				m_nBufOffset;
    OSMutex         m_mutex;
    http_method     m_method;
    HTTPCMD         m_cmd;
    TRANSMSG        m_transmsg;
    MEETMSG         m_meetmsg;
};

#endif	// __MsgServerMeeting_MRT_HTTP_SENDER_H__
