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
#include <string>
#include "XTcpClientImpl.h"
#include "XMsgProcesser.h"
#include "XMsgCallback.h"


class XMsgClient : public XTcpClientCallback{
public:
    XMsgClient();
    ~XMsgClient();
    
    int Init(XMsgCallback& cb, const std::string& server, int port, bool bAutoConnect=true);
    int Unin();
    
    int Login(const std::string& userid, const std::string& pass);
    int SndMsg(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg);
    int GetMsg(const std::string& userid, const std::string& pass);
    int Logout(const std::string& userid, const std::string& pass);
    
    int CreateRoom(const std::string& userid, const std::string& pass, const std::string& roomid);
    int EnterRoom(const std::string& userid, const std::string& pass, const std::string& roomid);
    int LeaveRoom(const std::string& userid, const std::string& pass, const std::string& roomid);
    int DestroyRoom(const std::string& userid, const std::string& pass, const std::string& roomid);
    int SndMsgTo(const std::string& userid, const std::string& pass, const std::string& roomid, const std::string& msg, const std::list<std::string>& ulist);
    
    TcpState Status() { if(m_pClient) return m_pClient->Status();else return NOT_CONNECTED; }
    
public:
    // For XTcpClientCallback
    virtual void OnServerConnected();
    virtual void OnServerDisconnect();
    virtual void OnServerConnectionFailure();
    
    virtual void OnTick();
    virtual void OnMessageSent(int err);
    virtual void OnMessageRecv(const char*pData, int nLen);
private:
    bool RefreshTime();
    int KeepAlive();
    int SendEncodeMsg(std::string& msg);
private:
    XTcpClient*              m_pClient;
    XMsgProcesser*           m_pMsgProcesser;
    uint32                   m_lastUpdateTime;
};



#endif /* defined(__dyncRTCMsgClient__XMsgClient__) */
