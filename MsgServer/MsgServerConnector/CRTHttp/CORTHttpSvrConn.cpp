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
    if (!sign || !meetingid || !pushMsg || !notification) {
        LE("HttpPushMeetingMsg params error\n");
        return;
    }
    if (strlen(pushMsg)>1024) {
        LE("HttpPushMeetingMsg Msg Len is over Max Len\n");
        return;
    }
    int outLen = 0;
    char data[1216] = {0};//1024+128+64:msg len + other value len + attr name len
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
    if (!sign || !targetid || !pushMsg || !notification) {
        LE("HttpPushMeetingMsg params error\n");
        return;
    }
    if (strlen(pushMsg)>1024) {
        LE(" HttpPushMeetingMsg Msg Len is over Max Len\n");
        return;
    }
    int outLen = 0;
    char data[1216] = {0};//1024+128+64:msg len + other value len + attr name len
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
void CORTHttpSvrConn::OnReadEvent(const char*data, int size)
{
	{//
        while ((m_nBufOffset + size) > m_nBufLen)
        {
            int newLen = m_nBufLen + kRequestBufferSizeInBytes;
            if (size > newLen)
                newLen = m_nBufLen + size;
            char* temp = new char[newLen];
            if (temp == NULL)
                continue;
            memcpy(temp, m_pBuffer, m_nBufLen);
            delete[] m_pBuffer;
            m_pBuffer = temp;
            m_nBufLen = newLen;
        }
        
        memcpy(m_pBuffer + m_nBufOffset, data, size);
        m_nBufOffset += size;
	}

	{// 
		int parsed = 0;
		parsed = CORTConnHttp::ProcessData(m_pBuffer, m_nBufOffset);
	
        if (parsed > 0 && m_pBuffer != NULL)
        {
            m_nBufOffset -= parsed;
            if (m_nBufOffset == 0)
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
    CORTHttpSender *sender = new CORTHttpSender();
    sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
    sender->SendRequest(pData, nLen);
    return 0;
}

