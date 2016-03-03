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
    
    int MCInit(XMsgCallback* cb, const std::string& uid, const std::string& token, const std::string& nname, const std::string& server="", int port=0);
    int MCUnin();
    
    int MCSndMsg(const std::string& roomid, const std::string& rname, const std::string& msg);
    int MCGetMsg(GETCMD cmd);
    
    int MCOptRoom(MEETCMD cmd, const std::string& roomid, const std::string& rname, const std::string& remain);
    int MCSndMsgTo(const std::string& roomid, const std::string& rname, const std::string& msg, const std::list<std::string>& ulist);
    
    int MCNotifyMsg(const std::string& roomid, const std::string& rname, SENDTAGS tags, const std::string& msg);
    
    int MCConnStatus() { return MSStatus(); }
    void MCSetNickName(const std::string& nickname) { SetNickName(nickname); }
    
//for XMsgCallback
public:
    virtual void OnSndMsg(const std::string& msg);
    virtual void OnGetMsg(const std::string& msg);
    virtual void OnMsgServerConnected();
    virtual void OnMsgServerDisconnect();
    virtual void OnMsgServerConnectionFailure();
    virtual void OnMsgServerState(MSState state);
    id<MsgClientProtocol> m_protocol;
};
#endif
