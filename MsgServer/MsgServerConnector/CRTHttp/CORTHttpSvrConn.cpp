#include <stdio.h>
#include "SocketUtils.h"
#include "CORTHttpSvrConn.h"
#include "CORTHttpSender.h"


CORTHttpSvrConn::CORTHttpSvrConn(void)
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

CORTHttpSvrConn::~CORTHttpSvrConn(void)
{
	delete[] m_pBuffer;
    m_pBuffer = NULL;
}

////////////////////////////////////////////////////
///////////////  write to http  ////////////////////
////////////////////////////////////////////////////

//* HTTP_POST

void CORTHttpSvrConn::HttpPushMeetingMsg(const char* sign, const char* meetingid, const char* pushMsg, const char* notification)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&meetingid=%s&pushMsg=%s&notification=%s", sign, meetingid, pushMsg, notification);
    const char* msg = GenerateRequest(HTTP_POST, "jpush/pushMeetingMsg", data, outLen);
    if (msg && outLen>0) {
        LI("CORTHttpSvrConn::HttpPushMeetingMsg ok\n");
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    } else {
        LE("CORTHttpSvrConn::HttpPushMeetingMsg error\n");
    }
}

void CORTHttpSvrConn::HttpPushCommonMsg(const char* sign, const char* targetid, const char* pushMsg, const char* notification)
{
    int outLen = 0;
    char data[512] = {0};
    sprintf(data, "sign=%s&targetid=%s&pushMsg=%s&notification=%s", sign, targetid, pushMsg, notification);
    const char* msg = GenerateRequest(HTTP_POST, "jpush/pushCommonMsg", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

////////////////////////////////////////////////////

//* For RCHttp
void CORTHttpSvrConn::OnReadEvent(const char*pData, int nLen)
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
		parsed = CORTConnHttp::ProcessData(m_pBuffer, m_nBufOffset);
	
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

int CORTHttpSvrConn::OnWriteEvent(const char*pData, int nLen, int* nOutLen)
{
    LI("CORTHttpSvrConn::OnWriteEvent enter\n");
    CORTHttpSender *sender = new CORTHttpSender();
    sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
    sender->SendRequest(pData, nLen);
    return 0;
}

