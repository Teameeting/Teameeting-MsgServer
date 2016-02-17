#ifndef __MsgServerConnector_CRT_HTTP_SVR_CONN_H__
#define __MsgServerConnector_CRT_HTTP_SVR_CONN_H__
#include "RTMessage.h"
#include "refcount.h"
#include "scoped_ref_ptr.h"
#include "RTHttpSender.h"
#include "RTHttpSvrConn.h"

#define C_HTTP_CMD_INVALID (0)

class CRTHttpSvrConn : public RTHttpSvrConn
{
public:
	CRTHttpSvrConn(void);
	virtual ~CRTHttpSvrConn(void);

public:
    void SetHttpHost(const std::string& addr, const unsigned short port, const std::string& host) {
        m_httpIp = addr;
        m_httpPort = port;
        m_httpHost = host;
        RTConnHttp::SetHttpHost(host);
    }
    //* HTTP_POST
    void HttpPushMeetingMsg(const char* sign, const char* meetingid, const char* pushMsg, const char* notification);
    void HttpPushCommonMsg(const char* sign, const char* targetid, const char* pushMsg, const char* notification);
    
    
public:
	//* For RCHttp
    virtual int  OnWriteEvent(const char*pData, int nLen, int* nOutLen);

public:
    class CSender : public RTHttpSender{
        //* For RTConnHttp
    public:
        CSender():RTHttpSender(){}
        virtual void OnResponse(const char*pData, int nLen);
    };
    
private:
    std::string           m_httpIp;
    unsigned short        m_httpPort;
    std::string           m_httpHost;
};

#endif	// __MsgServerConnector_CRT_HTTP_SVR_CONN_H__
