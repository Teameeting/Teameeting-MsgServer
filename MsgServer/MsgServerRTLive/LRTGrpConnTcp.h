//
//  LRTGrpConnTcp.h
//  MsgServerRTLive
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTGrpConnTcp__
#define __MsgServerRTLive__LRTGrpConnTcp__

#include <stdio.h>

#define DEF_PROTO 1
#include "ProtoCommon.h"

class LRTGrpConnTcp {
public:
    LRTGrpConnTcp();
    virtual ~LRTGrpConnTcp();

    int DoProcessData(const char* pData, int nLen);

    char* GenerateResponse(int code, const std::string&query, const char*pData, int nLen, int&outLen);
    void SendResponse(int code, const std::string&strContent);

public:
    virtual void OnSyncSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnSyncData(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnGroupNotify(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnCreateGroupSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnDeleteGroupSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg) = 0;
    virtual void OnResponse(const char*pData, int nLen)=0;

private:

};

#endif /* defined(__MsgServerRTLive__LRTGrpConnTcp__) */
