#ifndef __MsgServerMeeting_CORT_HTTP_SVR_CONN_H__
#define __MsgServerMeeting_CORT_HTTP_SVR_CONN_H__
#include "CORTHttp.h"
#include "CORTConnHttp.h"
#include "RTMessage.h"
#include "refcount.h"
#include "scoped_ref_ptr.h"

class CORTHttpSvrConn
	: public CORTHttp
	, public CORTConnHttp
    , public rtc::RefCountInterface
{
public:
	CORTHttpSvrConn(void);
	virtual ~CORTHttpSvrConn(void);

public:
    void SetHttpHost(const std::string& addr, const unsigned short port, const std::string& host) {
        m_httpIp = addr;
        m_httpPort = port;
        m_httpHost = host;
    }
    
    //* HTTP_POST
    void HttpPushMeetingMsg(const char* sign, const char* meetingid, const char* pushMsg, const char* notification);
    void HttpPushCommonMsg(const char* sign, const char* targetid, const char* pushMsg, const char* notification);
    
    void SendRequest(const char* pData, int nLen);
    
public:
	//* For RCHttp
    virtual void OnReadEvent(const char*data, int size);
    virtual int  OnWriteEvent(const char*pData, int nLen, int* nOutLen);
	virtual void OnTickEvent(){}

public:
    virtual void OnResponse(const char*pData, int nLen) {}
    virtual void OnSend(const char*pData, int nLen) {}
private:
	char			*m_pBuffer;
	int				m_nBufLen;
	int				m_nBufOffset;
    std::string      m_httpIp;
    unsigned short   m_httpPort;
    std::string      m_httpHost;
};

#endif	// __MsgServerMeeting_CORT_HTTP_SVR_CONN_H__
