//
//  XMsgProcesser.h
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef __dyncRTCMsgClient__XMsgProcesser__
#define __dyncRTCMsgClient__XMsgProcesser__

#include <stdio.h>
#include <iostream>
#include <vector>
#define DEF_PROTO 1
#if DEF_PROTO
#include "proto/common_msg.pb.h"
#include "proto/meet_msg.pb.h"
#include "proto/meet_msg_type.pb.h"
#else
#include "RTSignalMsg.h"
#include "RTMeetMsg.h"
#include "RTMessage.h"
#endif
#include "XMsgCallback.h"

class XMsgClientHelper;

class XMsgProcesser{
public:
    XMsgProcesser(XMsgCallback& cb, XMsgClientHelper& helper):m_callback(cb), m_helper(helper){}
    ~XMsgProcesser(){}
public:

    int EncodeLogin(std::string& outstr, const std::string& userid, const std::string& token, const std::string& nname);
    int EncodeSndMsg(std::string& outstr, const std::string& userid, const std::string& token, const std::string& nname, const std::string& roomid, const std::string& rname, const std::vector<std::string>& to, const std::string& msg, int tag, int type);
    int EncodeGetMsg(std::string& outstr, const std::string& userid, const std::string& token, int tag);
    int EncodeLogout(std::string& outstr, const std::string& userid, const std::string& token);
    int EncodeKeepAlive(std::string& outstr, const std::string& userid);

    int DecodeRecvData(const char* pData, int nLen);

public:
    //handle XTcpClient callback
    void ServerConnected();
    void ServerDisconnect();
    void ServerConnectionFailure();
    void ServerState(MSState state);

protected:
    int DecodeLogin(int code, const std::string& cont);
    int DecodeSndMsg(int code, const std::string& cont);
    int DecodeGetMsg(int code, const std::string& cont);
    int DecodeLogout(int code, const std::string& cont);
    int DecodeKeepAlive(int code, const std::string& cont);
private:
    XMsgCallback        &m_callback;
    XMsgClientHelper    &m_helper;
};

#endif /* defined(__dyncRTCMsgClient__XMsgProcesser__) */
