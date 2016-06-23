//
//  XGrpMsgClient.h
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef __dyncRTCMsgClient__XGrpMsgClient__
#define __dyncRTCMsgClient__XGrpMsgClient__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include "XTcpClientImpl.h"
#include "XGrpMsgProcesser.h"
#include "XMsgCallback.h"
#include "XJSBuffer.h"

class XGrpMsgClient
: public XTcpClientCallback
, public XGrpMsgClientHelper
, public XJSBuffer{
public:
    XGrpMsgClient();
    ~XGrpMsgClient();

    int Init(XMsgCallback* cb, const std::string& uid, const std::string& token, int module, const std::string& server="", int port=0, bool bAutoConnect=true);
    int Unin();

    int GroupNotify(const std::string& userid, const std::string& groupid);
    int GroupNotifys(const std::vector<std::string>& userids, const std::string& groupid);

    int GenSyncDataRequest(const std::string& userid, const std::string& groupid, long long seqn);
    int GenSyncDataRequests(const std::vector<std::string>& userids, const std::string& groupid, long long seqn);

    MSState MSStatus() { return m_msState; }

public:
    // For XTcpClientCallback
    virtual void OnServerConnected();
    virtual void OnServerDisconnect();
    virtual void OnServerConnectionFailure();

    virtual void OnTick();
    virtual void OnMessageSent(int err);
    virtual void OnMessageRecv(const char*pData, int nLen);

    // For XGrpMsgClientHelper
    virtual void OnLogin(int code, const std::string& userid);
    virtual void OnLogout(int code, const std::string& userid);
    virtual void OnKeepLive(int code, const std::string& cont);
    virtual void OnGroupNotify(int code, const std::string& cont);

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

    XMsgCallback*            m_pCallback;
    XTcpClientImpl*          m_pClientImpl;
    XGrpMsgProcesser*        m_pGrpMsgProcesser;
    uint32_t                 m_lastUpdateTime;
    std::string              m_uid;
    std::string              m_token;
    std::string              m_server;
    int                      m_port;
    bool                     m_autoConnect;
    bool                     m_login;
    MSState                  m_msState;
    long long                m_curSeqn;
    pms::EModuleType         m_module;
};

#endif /* defined(__dyncRTCMsgClient__XGrpMsgClient__) */
