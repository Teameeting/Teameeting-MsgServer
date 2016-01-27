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
#include <list>
#include "RTSignalMsg.h"
#include "RTMeetMsg.h"
#include "RTMessage.h"
#include "XMsgCallback.h"

class XMsgClientHelper;

class XMsgProcesser{
public:
    XMsgProcesser(XMsgCallback& cb, XMsgClientHelper& helper):m_callback(cb), m_helper(helper){}
    ~XMsgProcesser(){}
public:
    
    int EncodeLogin(std::string& outstr, const std::string& userid, const std::string& pass, const std::string& nname);
    int EncodeSndMsg(std::string& outstr, const std::string& userid, const std::string& pass, const std::string& nname, const std::string& roomid, const std::string& rname, const std::string& to, const std::string& msg, int cmd, int action, int tags, int type);
    int EncodeGetMsg(std::string& outstr, const std::string& userid, const std::string& pass, int cmd);
    int EncodeLogout(std::string& outstr, const std::string& userid, const std::string& pass);
    int EncodeKeepAlive(std::string& outstr, const std::string& userid);
    
    int DecodeRecvData(const char* pData, int nLen);
    int GetMemberToJson(const std::list<std::string> ulist, std::string& tousers);

public:
    //handle XTcpClient callback
    void ServerConnected();
    void ServerDisconnect();
    void ServerConnectionFailure();
    void ServerState(MSState state);
    
protected:
    int DecodeLogin(MEETMSG& msg);
    int DecodeSndMsg(MEETMSG& msg);
    int DecodeGetMsg(MEETMSG& msg);
    int DecodeLogout(MEETMSG& msg);
    int DecodeKeepAlive(MEETMSG& msg);
    long long GenericTransSeq();
private:
    XMsgCallback        &m_callback;
    XMsgClientHelper    &m_helper;
};

#endif /* defined(__dyncRTCMsgClient__XMsgProcesser__) */
