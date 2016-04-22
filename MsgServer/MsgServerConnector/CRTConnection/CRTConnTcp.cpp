//
//  CRTConnTcp.cpp
//  dyncRTConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <string.h>
#include "CRTConnTcp.h"
#include "rtklog.h"
#include "RTMsgCommon.h"

CRTConnTcp::CRTConnTcp()
{

}

CRTConnTcp::~CRTConnTcp()
{

}

int CRTConnTcp::DoProcessData(const char* pData, int nLen)
{
#if DEF_PROTO
    std::string msg(pData, nLen);
    pms::MsgReq request;
    if (!request.ParseFromString(msg)) {
        LE("request.ParseFromString error\n");
        return nLen;
    }

    if (request.svr_cmds() == pms::EServerCmd::CKEEPALIVE) {
        OnKeepAlive(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CLOGIN) {
        OnLogin(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CSNDMSG) {
        OnSndMsg(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CGETMSG) {
        OnGetMsg(request.svr_cmds(), request.mod_type(), request.content());
    } else if (request.svr_cmds() == pms::EServerCmd::CLOGOUT) {
        OnLogout(request.svr_cmds(), request.mod_type(), request.content());
    } else {
        LE("parse MsgReq params error\n");
    }
#else
    LE("not define DEF_PROTO\n");
#endif
    return nLen;
}


char* CRTConnTcp::GenerateResponse(int code, const std::string&query, const char*pData, int nLen, int&outLen)
{
    return (char*)"";
}


void CRTConnTcp::SendResponse(int code, const std::string&query, const char*pData, int nLen)
{

}

void CRTConnTcp::SendResponse(int code, const std::string&strContent)
{
    OnResponse(strContent.c_str(), (int)strContent.length());
}
