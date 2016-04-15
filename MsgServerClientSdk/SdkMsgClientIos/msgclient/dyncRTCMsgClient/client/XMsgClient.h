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
#include "XJSBuffer.h"

class XMsgClientHelper {
public:
    XMsgClientHelper() {}
    ~XMsgClientHelper() {}
    
    virtual void OnLogin(int code, const std::string& userid) = 0;
    virtual void OnLogout(int code, const std::string& userid) = 0;
};

class XMsgClient
: public XTcpClientCallback
, public XMsgClientHelper
, public XJSBuffer{
public:
    XMsgClient();
    ~XMsgClient();
    
    int Init(XMsgCallback* cb, const std::string& uid, const std::string& token, const std::string& nname, const std::string& server="", int port=0, bool bAutoConnect=true);
    int Unin();
    
    int SndMsg(const std::string& roomid, const std::string& rname, const std::string& msg);
    int GetMsg(GETCMD cmd);

    int OptRoom(MEETCMD cmd, const std::string& roomid, const std::string& rname, const std::string& remain);
    int SndMsgTo(const std::string& roomid, const std::string& rname, const std::string& msg, const std::list<std::string>& ulist);
    
    int NotifyMsg(const std::string& roomid, const std::string& rname, SENDTAGS tags, const std::string& msg);
    
    MSState MSStatus() { return m_msState; }
    void SetNickName(const std::string& nickname) { m_nname = nickname; }
    
public:
    // For XTcpClientCallback
    virtual void OnServerConnected();
    virtual void OnServerDisconnect();
    virtual void OnServerConnectionFailure();
    
    virtual void OnTick();
    virtual void OnMessageSent(int err);
    virtual void OnMessageRecv(const char*pData, int nLen);
    
    // For XMsgClientHelper
    virtual void OnLogin(int code, const std::string& userid);
    virtual void OnLogout(int code, const std::string& userid);
    
    // For XJSBuffer
    virtual void OnRecvMessage(const char*message, int nLen);
private:
    int Login();
    int Logout();
    bool RefreshTime();
    int KeepAlive();
    int SendEncodeMsg(std::string& msg);
    unsigned short readShort(char** pptr);
    void writeShort(char** pptr, unsigned short anInt);
private:
    XTcpClient*              m_pClient;
    XMsgProcesser*           m_pMsgProcesser;
    uint32_t                 m_lastUpdateTime;
    std::string              m_uid;
    std::string              m_token;
    std::string              m_nname;
    std::string              m_server;
    int                      m_port;
    bool                     m_autoConnect;
    bool                     m_login;
    MSState                  m_msState;
};



#endif /* defined(__dyncRTCMsgClient__XMsgClient__) */
