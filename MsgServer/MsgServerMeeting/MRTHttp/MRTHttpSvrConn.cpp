#include <stdio.h>
#include "SocketUtils.h"
#include "MRTHttpSvrConn.h"
#include "MRTHttpSender.h"


MRTHttpSvrConn::MRTHttpSvrConn(void)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
, m_httpHost("")
, m_httpIp("")
, m_httpPort(0)
{
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
}

MRTHttpSvrConn::~MRTHttpSvrConn(void)
{
	delete[] m_pBuffer;
    m_pBuffer = NULL;
}

////////////////////////////////////////////////////
///////////////  write to http  ////////////////////
////////////////////////////////////////////////////

//* HTTP_POST

void MRTHttpSvrConn::HttpInsertMeetingMsg(const char* sign, const char* meetingid, const char* messagetype, const char* sessionid, const char* strMsg, const char* userid)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s&messagetype=%s&sessionid=%s&strMsg=%s&userid=%s", sign, meetingid, messagetype, sessionid, strMsg, userid);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/insertMeetingMsg", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpInsertSessionMeetingInfo(const char* sign, const char* meetingid, const char* sessionid, const char* sessionstatus, const char* sessiontype, const char* sessionnumber)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s&sessionid=%s&sessionstatus=%s&sessiontype=%s&sessionnumber=%s", sign, meetingid, sessionid, sessionstatus, sessiontype, sessionnumber);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/insertSessionMeetingInfo", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpUpdateSessionMeetingEndtime(const char* sign, const char* sessionid)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&sessionid=%s", sign, sessionid);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/updateSessionMeetingEndtime", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpUpdateSessionMeetingNumber(const char* sign, const char* sessionid, const char* sessionnumber, const char* meetingid)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&sessionid=%s&sessionnumber=%s&meetingid=%s", sign, sessionid, sessionnumber, meetingid);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/updateSessionMeetingNumber", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void MRTHttpSvrConn::HttpUpdateUserMeetingJointime(const char* sign, const char* meetingid)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s", sign, meetingid);
    const char* msg = GenerateRequest(HTTP_POST, "meeting/updateUserMeetingJointime", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}


//* HTTP_GET
void MRTHttpSvrConn::HttpGetMeetingInfo(TRANSMSG& tmsg, MEETMSG& msg)
{
    int outLen = 0;
    char data[512] = {0};
    const char* meetingid = msg._room.c_str();
    sprintf(data, "meeting/getMeetingInfo/%s", meetingid);
    const char* pmsg = GenerateRequest(HTTP_GET, data, "", outLen);
    if (pmsg && outLen>0) {
        LI("HttpGetMeetingInfo send msg:%s\n", pmsg);
        MRTHttpSender *sender = new MRTHttpSender(HTTPCMD::_get_meeting_info, tmsg, msg);
        sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
        sender->SendRequest(pmsg, outLen);
        free((void*)pmsg);
        pmsg = NULL;
    }
}

void MRTHttpSvrConn::HttpGetMeetingInfo(TRANSMSG& tmsg, MEETMSG& msg) const
{
    int outLen = 0;
    char data[512] = {0};
    const char* meetingid = msg._room.c_str();
    sprintf(data, "meeting/getMeetingInfo/%s", meetingid);
    const char* pmsg = GenerateRequest(HTTP_GET, data, "", outLen);
    if (pmsg && outLen>0) {
        LI("HttpGetMeetingInfo send msg:%s\n", pmsg);
        MRTHttpSender *sender = new MRTHttpSender(HTTPCMD::_get_meeting_info, tmsg, msg);
        sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
        sender->SendRequest(pmsg, outLen);
        free((void*)pmsg);
        pmsg = NULL;
    }
}

void MRTHttpSvrConn::HttpGetMeetingMemberList(TRANSMSG& tmsg, MEETMSG& msg)
{
    int outLen = 0;
    char data[512] = {0};
    const char* meetingid = msg._room.c_str();
    sprintf(data, "meeting/getMeetingMemberList/%s", meetingid);
    const char* pmsg = GenerateRequest(HTTP_GET, data, "", outLen);
    if (pmsg && outLen>0) {
        LI("HttpGetMeetingMemberList send msg:%s\n", pmsg);
        MRTHttpSender *sender = new MRTHttpSender(HTTPCMD::_get_member_list, tmsg, msg);
        sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
        sender->SendRequest(pmsg, outLen);
        free((void*)pmsg);
        pmsg = NULL;
    }
}

void MRTHttpSvrConn::HttpGetMeetingMemberList(TRANSMSG& tmsg, MEETMSG& msg) const
{
    int outLen = 0;
    char data[512] = {0};
    const char* meetingid = msg._room.c_str();
    sprintf(data, "meeting/getMeetingMemberList/%s", meetingid);
    const char* pmsg = GenerateRequest(HTTP_GET, data, "", outLen);
    if (pmsg && outLen>0) {
        LI("HttpGetMeetingMemberList send msg:%s\n", pmsg);
        MRTHttpSender *sender = new MRTHttpSender(HTTPCMD::_get_member_list, tmsg, msg);
        sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
        sender->SendRequest(pmsg, outLen);
        free((void*)pmsg);
        pmsg = NULL;
    }
}


////////////////////////////////////////////////////

//* For RCHttp
void MRTHttpSvrConn::OnReadEvent(const char*pData, int nLen)
{
	{//
		while((m_nBufOffset + nLen) > m_nBufLen)
		{
			m_nBufLen += kRequestBufferSizeInBytes;
			char* ptr = (char *)realloc(m_pBuffer, m_nBufLen);
			if(ptr != NULL)
			{//
				m_pBuffer = ptr;
			}
			else
			{//
				m_nBufLen -= kRequestBufferSizeInBytes;
				continue;
			}
		}

		memcpy(m_pBuffer + m_nBufOffset, pData, nLen);
		m_nBufOffset += nLen;
	}

	{// 
		int parsed = 0;
		parsed = MRTConnHttp::ProcessData(m_pBuffer, m_nBufOffset);
	
		if(parsed > 0)
		{
			m_nBufOffset -= parsed;
			if(m_nBufOffset == 0)
			{
				memset(m_pBuffer, 0, m_nBufLen);
			}
			else
			{
				memmove(m_pBuffer, m_pBuffer + parsed, m_nBufOffset);
			}
		}
	}
}

int MRTHttpSvrConn::OnWriteEvent(const char*pData, int nLen, int* nOutLen)
{
    MRTHttpSender *sender = new MRTHttpSender();
    sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
    sender->SendRequest(pData, nLen);
    return 0;
}

