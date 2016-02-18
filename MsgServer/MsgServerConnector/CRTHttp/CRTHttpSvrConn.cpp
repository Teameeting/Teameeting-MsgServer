#include <stdio.h>
#include "SocketUtils.h"
#include "CRTHttpSvrConn.h"
#include "RTConnHttp.h"


CRTHttpSvrConn::CRTHttpSvrConn(void)
: RTHttpSvrConn()
, m_httpHost("")
, m_httpIp("")
, m_httpPort(0)
{
	
}

CRTHttpSvrConn::~CRTHttpSvrConn(void)
{
    
}

////////////////////////////////////////////////////
///////////////  write to http  ////////////////////
////////////////////////////////////////////////////

//* HTTP_POST

void CRTHttpSvrConn::HttpPushMeetingMsg(const char* sign, const char* meetingid, const char* pushMsg, const char* notification)
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
        LI("CRTHttpSvrConn::HttpPushMeetingMsg ok\n");
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    } else {
        LE("CRTHttpSvrConn::HttpPushMeetingMsg error\n");
    }
}

void CRTHttpSvrConn::HttpPushCommonMsg(const char* sign, const char* targetid, const char* pushMsg, const char* notification)
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

int CRTHttpSvrConn::OnWriteEvent(const char*pData, int nLen, int* nOutLen)
{
    CSender *sender = new CSender();
    sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
    sender->SendRequest(pData, nLen);
    return 0;
}

void CRTHttpSvrConn::CSender::OnResponse(const char* pData, int nLen)
{
    LI("CRTHttpSvrConn::CSender::OnResponse nLen:%d, pData:%s\n", nLen, pData);
    this->Signal(kKillEvent);
}

