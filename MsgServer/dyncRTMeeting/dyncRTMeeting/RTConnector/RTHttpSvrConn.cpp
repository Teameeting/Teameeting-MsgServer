#include <stdio.h>
#include "SocketUtils.h"
#include "RTHttpSvrConn.h"
#include "RTHttpSender.h"

RTHttpSvrConn::RTHttpSvrConn(void)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
{
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
}

RTHttpSvrConn::~RTHttpSvrConn(void)
{
	delete[] m_pBuffer;
    m_pBuffer = NULL;
}

////////////////////////////////////////////////////
///////////////  write to http  ////////////////////
////////////////////////////////////////////////////

void RTHttpSvrConn::HttpUpdateRoomMemNumber(const char* sign, const char* meetingid, const char* meetingMemNumber)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s&meetingMemNumber=%s", sign, meetingid, meetingMemNumber);
    const char* msg = GenerateRequest("meeting/updateRoomMemNumber", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void RTHttpSvrConn::HttpInsertMeetingMsg(const char* sign, const char* meetingid, const char* messageid, const char* messagetype, const char* sessionid, const char* strMsg, const char* userid)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s&messageid=%s&messagetype=%s&sessionid=%s&strMsg=%s&userid=%s", sign, meetingid, messageid, messagetype, sessionid, strMsg, userid);
    const char* msg = GenerateRequest("meeting/insertMeetingMsg", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void RTHttpSvrConn::HttpInsertSessionMeetingInfo(const char* sign, const char* meetingid, const char* sessionid, const char* sessionstatus, const char* sessiontype, const char* sessionnumber)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s&sessionid=%s&sessionstatus=%s&sessiontype=%s&sessionnumber=%s", sign, meetingid, sessionid, sessionstatus, sessiontype, sessionnumber);
    const char* msg = GenerateRequest("meeting/insertSessionMeetingInfo", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void RTHttpSvrConn::HttpUpdateSessionMeetingEndtime(const char* sign, const char* sessionid)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&sessionid=%s", sign, sessionid);
    const char* msg = GenerateRequest("meeting/updateSessionMeetingEndtime", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void RTHttpSvrConn::HttpUpdateSessionMeetingNumber(const char* sign, const char* sessionid, const char* sessionnumber)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&sessionid=%s&sessionnumber=%s", sign, sessionid, sessionnumber);
    const char* msg = GenerateRequest("meeting/updateSessionMeetingNumber", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void RTHttpSvrConn::HttpUpdateUserMeetingJointime(const char* sign, const char* meetingid)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s", sign, meetingid);
    const char* msg = GenerateRequest("meeting/updateUserMeetingJointime", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

void RTHttpSvrConn::HttpInsertUserMeetingRoom(const char* sign, const char* meetingid)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s", sign, meetingid);
    const char* msg = GenerateRequest("meeting/insertUserMeetingRoom", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

////////////////////////////////////////////////////

//* For RCHttp
void RTHttpSvrConn::OnReadEvent(const char*pData, int nLen)
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
		parsed = RTConnHttp::ProcessData(m_pBuffer, m_nBufOffset);
	
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

int RTHttpSvrConn::OnWriteEvent(const char*pData, int nLen, int* nOutLen)
{
    //RTHttpSender *sender = new RTHttpSender();
    //sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
    //sender->SendRequest(pData, nLen);
    return 0;
}

