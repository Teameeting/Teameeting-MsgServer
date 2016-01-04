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

class XMsgProcesser{
public:
    XMsgProcesser(XMsgCallback& cb):m_callback(cb){}
    ~XMsgProcesser(){}
public:
    int     _cmd;
    int     _action;
    int     _tags;
    int     _type;
    long long   _mseq;
    std::string _from;
    std::string _room;
    std::string _sess;
    std::string _to;
    std::string _cont;
    std::string _pass;
    int     _code;
    std::string _status;
    int EncodeLogin(std::string& outstr, const std::string& userid, const std::string& pass);
    int EncodeSndMsg(std::string& outstr, const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& to, const std::string& msg, int cmd, int action, int tags, int type);
    int EncodeGetMsg(std::string& outstr, const std::string& userid, const std::string& pass);
    int EncodeLogout(std::string& outstr, const std::string& userid, const std::string& pass);
    int EncodeKeepAlive(std::string& outstr);
    
    int DecodeRecvData(const char* pData, int nLen);
    int GetMemberToJson(const std::list<std::string> ulist, std::string& tousers);

public:
    //handle XTcpClient callback
    void ServerConnected();
    void ServerDisconnect();
    void ServerConnectionFailure();
protected:
    int DecodeLogin(SIGNALTYPE stype, MEETMSG& msg);
    int DecodeSndMsg(SIGNALTYPE stype, MEETMSG& msg);
    int DecodeGetMsg(SIGNALTYPE stype, MEETMSG& msg);
    int DecodeLogout(SIGNALTYPE stype, MEETMSG& msg);
    int DecodeKeepAlive(MEETMSG& msg);
    long long GenericTransSeq();
private:
    XMsgCallback    &m_callback;
};

#endif /* defined(__dyncRTCMsgClient__XMsgProcesser__) */
