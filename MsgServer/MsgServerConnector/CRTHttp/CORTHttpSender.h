#ifndef __MsgServerMeeting_CORT_HTTP_SENDER_H__
#define __MsgServerMeeting_CORT_HTTP_SENDER_H__
#include "RTTcp.h"
#include "CORTConnHttp.h"
#include "RTMessage.h"
#include "CORTHttpCmd.h"

class CORTHttpSender
	: public RTTcp
	, public CORTConnHttp
{
public:
	CORTHttpSender(void);
    CORTHttpSender(HTTPCMD cmd, TRANSMSG& tmsg, MEETMSG& msg);
	virtual ~CORTHttpSender(void);

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
    http_method     m_method;
    HTTPCMD         m_cmd;
    TRANSMSG        m_transmsg;
    MEETMSG         m_meetmsg;
};

#endif	// __MsgServerMeeting_CORT_HTTP_SENDER_H__
