#ifndef __MsgServerMeeting_MRT_HTTP_SVR_CONN_H__
#define __MsgServerMeeting_MRT_HTTP_SVR_CONN_H__
#include "MRTHttp.h"
#include "MRTConnHttp.h"
#include "OSMutex.h"
#include "RTMessage.h"
#include "refcount.h"
#include "scoped_ref_ptr.h"

class MRTHttpSvrConn
	: public MRTHttp
	, public MRTConnHttp
    , public rtc::RefCountInterface
{
public:
	MRTHttpSvrConn(void);
	virtual ~MRTHttpSvrConn(void);

public:
    void SetHttpHost(const std::string& addr, const unsigned short port, const std::string& host) {
        m_httpIp = addr;
        m_httpPort = port;
        m_httpHost = host;
    }
    
    //* HTTP_POST
    void HttpInsertMeetingMsg(const char* sign, const char* meetingid, const char* messagetype, const char* sessionid, const char* strMsg, const char* userid);
    void HttpInsertSessionMeetingInfo(const char* sign, const char* meetingid, const char* sessionid, const char* sessionstatus, const char* sessiontype, const char* sessionnumber);
    void HttpUpdateSessionMeetingEndtime(const char* sign, const char* sessionid);
    void HttpUpdateSessionMeetingNumber(const char* sign, const char* sessionid, const char* sessionnumber, const char* meetingid);
    void HttpUpdateUserMeetingJointime(const char* sign, const char* meetingid);
    void HttpUpdateSessionMeetingStatus(const char* sign, const char* sessionid, const char* sessionstatus);
    
    //* HTTP_GET
    void HttpGetMeetingInfo(TRANSMSG& tmsg, MEETMSG& msg);
    void HttpGetMeetingInfo(TRANSMSG& tmsg, MEETMSG& msg) const;
    void HttpGetMeetingMemberList(TRANSMSG& tmsg, MEETMSG& msg);
    void HttpGetMeetingMemberList(TRANSMSG& tmsg, MEETMSG& msg) const;
    
    void SendRequest(const char* pData, int nLen);
    
public:
	//* For RCHttp
    virtual void OnReadEvent(const char*pData, int nLen);
    virtual int  OnWriteEvent(const char*pData, int nLen, int* nOutLen);
	virtual void OnTickEvent(){}

public:
    virtual void OnResponse(const char*pData, int nLen) {}
    virtual void OnSend(const char*pData, int nLen) {}
private:
	char			 *m_pBuffer;
	int				 m_nBufLen;
	int				 m_nBufOffset;
    std::string      m_httpIp;
    unsigned short   m_httpPort;
    std::string      m_httpHost;
};

#endif	// __MsgServerMeeting_MRT_HTTP_SVR_CONN_H__
