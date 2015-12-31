//
//  MsgClient.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_MsgClient_h
#define MsgClientIos_MsgClient_h

#import <Foundation/Foundation.h>
#import "MsgClientProtocol.h"

#include "XMsgClient.h"
#include "XMsgCallback.h"


class MsgClient : public XMsgClient, public XMsgCallback{
public:
    MsgClient(id<MsgClientProtocol> protocol);
    virtual ~MsgClient();
    
    int MCInit(XMsgCallback& cb, const std::string& server, int port);
    int MCUnin();
    
    int MCLogin(const std::string& userid, const std::string& pass);
    int MCSndMsg(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg);
    int MCGetMsg(const std::string& userid, const std::string& pass);
    int MCLogout(const std::string& userid, const std::string& pass);
    
    int MCCreateRoom(const std::string& userid, const std::string& pass, const std::string& roomid);
    int MCEnterRoom(const std::string& userid, const std::string& pass, const std::string& roomid);
    int MCLeaveRoom(const std::string& userid, const std::string& pass, const std::string& roomid);
    int MCDestroyRoom(const std::string& userid, const std::string& pass, const std::string& roomid);
    int MCSndMsgTo(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg, const std::list<std::string>& ulist);
    
    int MCConnStatus() { return Status(); }
    
//for XMsgCallback
public:
    virtual void OnLogin(int code);
    virtual void OnSndMsg(const std::string msg);
    virtual void OnGetMsg(const std::string from, const std::string msg);
    virtual void OnLogout(int code);
    
    id<MsgClientProtocol> m_protocol;
};
#endif
