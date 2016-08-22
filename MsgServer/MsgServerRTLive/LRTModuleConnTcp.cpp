//
//  LRTModuleConnTcp.cpp
//  dyncRTRTLive
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <string.h>
#include "LRTModuleConnTcp.h"
#include "rtklog.h"

LRTModuleConnTcp::LRTModuleConnTcp()
{

}

LRTModuleConnTcp::~LRTModuleConnTcp()
{

}

int LRTModuleConnTcp::DoProcessData(const char* pData, int nLen)
{
#if DEF_PROTO
    std::string msg(pData, nLen);
    pms::MsgReq request;
    if (!request.ParseFromString(msg)) {
        LE("request.ParseFromString error\n");
        return nLen;
    }

    LI("LRTModuleConnTcp::DoProcessData request.svr_cmds:%d\n", request.svr_cmds());
    if (request.svr_cmds() == pms::EServerCmd::CGROUPNOTIFY) {
        OnGroupNotify(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CPGETDATA) {
        OnPGetData(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CCREATESEQN) {
        OnCreateGroupSeqn(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CDELETESEQN) {
        OnDeleteGroupSeqn(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CSYNCSEQN) {
        OnSyncSeqn(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CSYNCDATA) {
        OnSyncData(request.svr_cmds(), request.mod_type(), request.content());
    } else {
        LE("parse MsgReq params svr_cmds not handle:%d\n", request.svr_cmds());
    }
#else
    LE("not define DEF_PROTO\n");
#endif
    return nLen;
}

char* LRTModuleConnTcp::GenerateResponse(int code, const std::string&query, const char*pData, int nLen, int&outLen)
{
    return (char*)"";
}

void LRTModuleConnTcp::SendResponse(int code, const std::string&strContent)
{
    OnResponse(strContent.c_str(), (int)strContent.length());
}
