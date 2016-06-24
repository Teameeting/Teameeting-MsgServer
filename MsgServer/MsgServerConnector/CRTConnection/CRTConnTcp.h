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

#define DEF_PROTO 1
#include "ProtoCommon.h"

class CRTConnTcp {
public:
    CRTConnTcp();
    virtual ~CRTConnTcp();

    int DoProcessData(const char* pData, int nLen);

    char* GenerateResponse(int code, const std::string&query, const char*pData, int nLen, int&outLen);
    void SendResponse(int code, const std::string&query, const char*pData, int nLen);
    void SendResponse(int code, const std::string&strContent);

public:
    virtual void OnLogin(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnSndMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnGetMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnLogout(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnKeepAlive(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnSyncSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnSyncData(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnResponse(const char*pData, int nLen)=0;

private:

};

#endif /* defined(__MsgServerConnector__CRTConnTcp__) */
