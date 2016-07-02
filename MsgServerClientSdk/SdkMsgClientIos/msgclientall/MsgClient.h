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
#import "MSTxtMessageDelegate.h"
#import "MSGroupDelegate.h"
#import "MSClientDelegate.h"
#import "MSSqlite3Manager.h"

#include "XMsgClient.h"
#include "XMsgCallback.h"

#include "RTSingleton.h"

class MsgClient : public XMsgClient, public XMsgCallback, public RTSingleton<MsgClient>{
    friend class RTSingleton<MsgClient>;
public:
    int MCInit(const std::string& uid, const std::string& token, const std::string& nname);
    int MCUnin();
    
    void MCSetTxtMsgDelegate(id<MSTxtMessageDelegate> txtMsgDelegate)
    {
        m_txtMsgDelegate = txtMsgDelegate;
    }
    
    void MCSetGroupDelegate(id<MSGroupDelegate> groupDelegate)
    {
        m_groupDelegate = groupDelegate;
    }
    
    void MCSetClientDelegate(id<MSClientDelegate> clientDelegate)
    {
        m_clientDelegate = clientDelegate;
    }
    
    int MCRegisterMsgCb(XMsgCallback* cb);
    int MCUnRegisterMsgCb(XMsgCallback* cb);
    int MCConnToServer(const std::string& server="", int port=0);
    
    int MCAddGroup(const std::string& groupid);
    int MCRmvGroup(const std::string& groupid);
    
    int MCSyncMsg();
    int MCSendTxtMsg(const std::string& groupid, const std::string& content);
    int MCSendTxtMsgToUsr(const std::string& userid, const std::string& content);
    int MCSendTxtMsgToUsrs(const std::vector<std::string>& vusrs, const std::string& content);
    
    int MCNotifyLive(const std::string& groupid, const std::string& hostid);
    int MCNotifyRedEnvelope(const std::string& groupid, const std::string& hostid);
    int MCNotifyBlacklist(const std::string& groupid, const std::string& userid);
    int MCNotifyForbidden(const std::string& groupid, const std::string& userid);
    int MCNotifySettedMgr(const std::string& groupid, const std::string& userid);
    
    int MCConnStatus() { return MSStatus(); }
    void MCSetNickName(const std::string& nickname) { SetNickName(nickname); }
    
//for XMsgCallback
public:
    virtual void OnSndMsg(int code, const std::string& msgid);
    virtual void OnCmdGroup(int code, int cmd, const std::string& groupid, const MSCbData& data);
    virtual void OnRecvMsg(const std::string& msg);
    virtual void OnRecvGroupMsg(const std::string& msg);
    
    virtual void OnSyncSeqn(int64 seqn);
    virtual void OnSyncGroupSeqn(const std::string& groupid, int64 seqn);
    
    virtual void OnMsgServerConnected();
    virtual void OnMsgServerConnecting();
    virtual void OnMsgServerDisconnect();
    virtual void OnMsgServerConnectionFailure();
protected:
    MsgClient():m_txtMsgDelegate(nullptr)
    , m_groupDelegate(nullptr)
    , m_clientDelegate(nullptr)
    , m_sqlite3Manager(nullptr){}
    ~MsgClient(){}
    
private:
    id<MSTxtMessageDelegate>    m_txtMsgDelegate;
    id<MSGroupDelegate>         m_groupDelegate;
    id<MSClientDelegate>        m_clientDelegate;
    
    MSSqlite3Manager*           m_sqlite3Manager;
    
    NSString*                   m_userId;
    NSString*                   m_token;
    NSString*                   m_nname;
};
#endif
