//
//  XMsgClient.h
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef __dyncRTCMsgClient__XMsgClient__
#define __dyncRTCMsgClient__XMsgClient__

#include <stdio.h>
#include <iostream>
#include "XTcpClientImpl.h"
#include "XMsgProcesser.h"
#include "XMsgCallback.h"

class XMsgClientHelper {
public:
    XMsgClientHelper() {}
    ~XMsgClientHelper() {}
    
    virtual void OnLogin(int code, const std::string& status, const std::string& userid) = 0;
    virtual void OnLogout(int code, const std::string& status, const std::string& userid) = 0;
};

class XMsgClient : public XTcpClientCallback, public XMsgClientHelper{
public:
    XMsgClient();
    ~XMsgClient();
    
    int Init(XMsgCallback& cb, const std::string& uid, const std::string& token, const std::string& server, int port, bool bAutoConnect=true);
    int Unin();
    
    int SndMsg(const std::string& roomid, const std::string& msg);
    int GetMsg(GETCMD cmd);

    int OptRoom(MEETCMD cmd, const std::string& roomid, const std::string& remain);
    int SndMsgTo(const std::string& roomid, const std::string& msg, const std::list<std::string>& ulist);
    
    int NotifyMsg(const std::string& roomid, const std::string& msg);
    
    TcpState Status() { if(m_pClient) return m_pClient->Status();else return NOT_CONNECTED; }
    
public:
    // For XTcpClientCallback
    virtual void OnServerConnected();
    virtual void OnServerDisconnect();
    virtual void OnServerConnectionFailure();
    
    virtual void OnTick();
    virtual void OnMessageSent(int err);
    virtual void OnMessageRecv(const char*pData, int nLen);
    
    // For XMsgClientHelper
    virtual void OnLogin(int code, const std::string& status, const std::string& userid);
    virtual void OnLogout(int code, const std::string& status, const std::string& userid);
private:
    int Login();
    int Logout();
    bool RefreshTime();
    int KeepAlive();
    int SendEncodeMsg(std::string& msg);
private:
    XTcpClient*              m_pClient;
    XMsgProcesser*           m_pMsgProcesser;
    uint32                   m_lastUpdateTime;
    std::string              m_Uid;
    std::string              m_Token;
};



#endif /* defined(__dyncRTCMsgClient__XMsgClient__) */
