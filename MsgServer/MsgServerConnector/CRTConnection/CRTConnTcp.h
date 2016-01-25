//
//  CRTConnTcp.h
//  MsgServerConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTConnTcp__
#define __MsgServerConnector__CRTConnTcp__

#include <stdio.h>
#include "RTMessage.h"
#include "RTMsg.h"
#include "RTMeetMsg.h"
#include "RTSignalMsg.h"
#include "OSMutex.h"

class CRTConnTcp {
public:
    CRTConnTcp();
    virtual ~CRTConnTcp();

    int ProcessData(const char*pData, int nLen);

    char* GenerateResponse(int code, const std::string&query, const char*pData, int nLen, int&outLen);
    void SendResponse(int code, const std::string&query, const char*pData, int nLen);
    void SendResponse(int code, const std::string&strContent);

public:
    virtual void OnLogin(const char* pUserid, const char* pPass, const char* pNname) = 0;
    virtual void OnSndMsg(MSGTYPE mType, long long mseq, const char* pUserid, const char* pData, int dLen) = 0;
    virtual void OnGetMsg(MSGTYPE mType, long long mseq, const char* pUserid) = 0;
    virtual void OnLogout(const char* pUserid) = 0;
    virtual void OnKeepAlive(const char* pUserid) = 0;
    virtual void OnResponse(const char*pData, int nLen)=0;

private:
    int DoProcessData(const char* pData, int nLen);
private:
    OSMutex         m_mutexMsg;
};

#endif /* defined(__MsgServerConnector__CRTConnTcp__) */
