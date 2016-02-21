#ifndef __MsgServerDispatcher_DRT_HTTP_SVR_CONN_H__
#define __MsgServerDispatcher_DRT_HTTP_SVR_CONN_H__
#include "RTMessage.h"
#include "refcount.h"
#include "scoped_ref_ptr.h"
#include "RTHttpSender.h"
#include "RTHttpSvrConn.h"

#define C_HTTP_CMD_INVALID (0)

class DRTHttpSvrConn : public RTHttpSvrConn
{
public:
	DRTHttpSvrConn(void);
	virtual ~DRTHttpSvrConn(void);

public:
    void SetHttpHost(const std::string& addr, const unsigned short port, const std::string& host) {
        m_httpIp = addr;
        m_httpPort = port;
        m_httpHost = host;
        RTConnHttp::SetHttpHost(host);
    }
    //* HTTP_POST
    void HttpPushMeetingMsg(const char* sign, const char* meetingid, const char* pushMsg, const char* notification, const char* extra);
    void HttpPushCommonMsg(const char* sign, const char* targetid, const char* pushMsg, const char* notification, const char* extra);
    
    
public:
	//* For RCHttp
    virtual int  OnWriteEvent(const char*pData, int nLen, int* nOutLen);

public:
    class DSender : public RTHttpSender{
        //* For RTConnHttp
    public:
        DSender():RTHttpSender(){}
        virtual void OnResponse(const char*pData, int nLen);
    };
    
private:
    std::string           m_httpIp;
    unsigned short        m_httpPort;
    std::string           m_httpHost;
};

#endif	// __MsgServerDispatcher_DRT_HTTP_SVR_CONN_H__
