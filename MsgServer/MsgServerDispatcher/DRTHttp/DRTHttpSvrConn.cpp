#include <stdio.h>
#include "SocketUtils.h"
#include "DRTHttpSvrConn.h"
#include "RTConnHttp.h"


DRTHttpSvrConn::DRTHttpSvrConn(void)
: RTHttpSvrConn()
, m_httpHost("")
, m_httpIp("")
, m_httpPort(0)
{
	
}

DRTHttpSvrConn::~DRTHttpSvrConn(void)
{
    
}

////////////////////////////////////////////////////
///////////////  write to http  ////////////////////
////////////////////////////////////////////////////

//* HTTP_POST

void DRTHttpSvrConn::HttpPushMeetingMsg(const char* sign, const char* meetingid, const char* pushMsg, const char* notification, const char* extra)
{
    if (!sign || !meetingid || !pushMsg || !notification || !extra || strlen(pushMsg)>1024) {
        LE("HttpPushMeetingMsg params error\n");
        return;
    }

    int outLen = 0;
    char data[1216] = {0};//1024+128+64:msg len + other value len + attr name len
    sprintf(data, "sign=%s&meetingid=%s&pushMsg=%s&notification=%s&extra=%s", sign, meetingid, pushMsg, notification, extra);
    const char* msg = GenerateRequest(HTTP_POST, "jpush/pushMeetingMsg", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    } else {
        LE("DRTHttpSvrConn::HttpPushMeetingMsg error\n");
    }
}

void DRTHttpSvrConn::HttpPushCommonMsg(const char* sign, const char* targetid, const char* pushMsg, const char* notification, const char* extra)
{
    if (!sign || !targetid || !pushMsg || !notification || !extra || strlen(pushMsg)>1024) {
        LE("HttpPushMeetingMsg params error\n");
        return;
    }

    int outLen = 0;
    char data[1216] = {0};//1024+128+64:msg len + other value len + attr name len
    sprintf(data, "sign=%s&targetid=%s&pushMsg=%s&notification=%s&extra=%s", sign, targetid, pushMsg, notification, extra);
    const char* msg = GenerateRequest(HTTP_POST, "jpush/pushCommonMsg", data, outLen);
    if (msg && outLen>0) {
        SendData(msg, outLen);
        free((void*)msg);
        msg = NULL;
    }
}

////////////////////////////////////////////////////

int DRTHttpSvrConn::OnWriteEvent(const char*pData, int nLen, int* nOutLen)
{
    DSender *sender = new DSender();
    sender->ConnHttpHost(m_httpIp, m_httpPort, m_httpHost);
    sender->SendRequest(pData, nLen);
    return 0;
}

void DRTHttpSvrConn::DSender::OnResponse(const char* pData, int nLen)
{
    this->Signal(kKillEvent);
}

