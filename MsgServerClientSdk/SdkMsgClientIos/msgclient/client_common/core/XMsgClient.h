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
#include <map>
#include <set>

#include "core/XTcpClientImpl.h"
#include "core/XMsgProcesser.h"
#include "core/XMsgCallback.h"
#include "core/XJSBuffer.h"

#include "RTMsgCommon.h"

class XMsgClientHelper {
public:
    XMsgClientHelper() {}
    ~XMsgClientHelper() {}

    virtual void OnHelpLogin(int code, const std::string& userid) = 0;
    virtual void OnHelpLogout(int code, const std::string& userid) = 0;
    virtual void OnHelpSndMsg(int code, const std::string& cont) = 0;
    virtual void OnHelpKeepLive(int code, const std::string& cont) = 0;
    virtual void OnHelpSyncSeqn(int code, const std::string& cont) = 0;
    virtual void OnHelpSyncData(int code, const std::string& cont) = 0;
    virtual void OnHelpSyncGroupData(int code, const std::string& cont) = 0;
    
    virtual void OnHelpGroupNotify(int code, const std::string& cont) = 0;
    virtual void OnHelpNotifySeqn(int code, const std::string& cont) = 0;
    virtual void OnHelpNotifyData(int code, const std::string& cont) = 0;
};

class XMsgClient
: public XTcpClientCallback
, public XMsgClientHelper
, public XJSBuffer{
public:
    XMsgClient();
    ~XMsgClient();

    int Init(const std::string& uid, const std::string& token, const std::string& nname, int module);
    int Unin();

    int RegisterMsgCb(XMsgCallback* cb);
    int UnRegisterMsgCb(XMsgCallback* cb);

    int ConnToServer(const std::string& server="", int port=0, bool bAutoConnect=true);
    
    int AddGroup(const std::string& groupid);
    int RmvGroup(const std::string& groupid);

    int SndMsg(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag);
    int SndMsgTo(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, const std::vector<std::string>& uvec);
    
    int FetchSeqn();
    int SyncSeqn(int64 seqn, int role);
    int SyncData(int64 seqn);
    int FetchGroupSeqn(const std::string& groupid);
    int SyncGroupSeqn(const std::string& groupid, int64 seqn, int role);
    int SyncGroupData(const std::string& gropuid, int64 seqn);

    MSState MSStatus() { return m_msState; }
    void SetNickName(const std::string& nickname) { m_nname = nickname; }
    void SetUIconUrl(const std::string& uiconurl) { m_uicon = uiconurl; }

public:
    // For XTcpClientCallback
    virtual void OnServerConnected();
    virtual void OnServerDisconnect();
    virtual void OnServerConnectionFailure();

    virtual void OnTick();
    virtual void OnMessageSent(int err);
    virtual void OnMessageRecv(const char*pData, int nLen);

    // For XMsgClientHelper
    virtual void OnHelpLogin(int code, const std::string& userid);
    virtual void OnHelpLogout(int code, const std::string& userid);
    virtual void OnHelpSndMsg(int code, const std::string& cont);
    virtual void OnHelpKeepLive(int code, const std::string& cont);
    virtual void OnHelpSyncSeqn(int code, const std::string& cont);
    virtual void OnHelpSyncData(int code, const std::string& cont);
    virtual void OnHelpGroupNotify(int code, const std::string& cont);
    virtual void OnHelpSyncGroupData(int code, const std::string& cont);
    virtual void OnHelpNotifySeqn(int code, const std::string& cont);
    virtual void OnHelpNotifyData(int code, const std::string& cont);

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
    XMsgProcesser*           m_pMsgProcesser;
    uint32_t                 m_lastUpdateTime;
    std::string              m_uid;
    std::string              m_grpid;
    std::string              m_token;
    std::string              m_nname;
    std::string              m_uicon;
    std::string              m_server;
    std::string              m_version;
    int                      m_port;
    bool                     m_autoConnect;
    bool                     m_login;
    MSState                  m_msState;
    pms::EModuleType         m_module;

};



#endif /* defined(__dyncRTCMsgClient__XMsgClient__) */
