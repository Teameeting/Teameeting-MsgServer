//
//  ClientSession.h
//  SeqnClient
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __SeqnClient__ClientSession__
#define __SeqnClient__ClientSession__

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include "MSGroupDelegate.h"
#include "MSClientDelegate.h"
#include "MSSubMessageDelegate.h"
#include "MSGroupManager.h"
#include "MSClientManager.h"
#include "MSMessageManager.h"

#define DEF_PROTO 1
#include "client_common/proto/common_msg.pb.h"
#include "client_common/proto/entity_msg.pb.h"
#include "client_common/proto/entity_msg_type.pb.h"
#include "client_common/proto/sys_msg.pb.h"
#include "client_common/proto/sys_msg_type.pb.h"

class ClientSession
    : public MSGroupDelegate
    , public MSClientDelegate
    , public MSSubMessageDelegate {
public:
    ClientSession();
    virtual ~ClientSession();

public:
    // from MSGroupDelegate
    virtual void OnAddGroupSuccess(const std::string& grpId);
    virtual void OnAddGroupFailed(const std::string& grpId, const std::string& reason, int code);
    virtual void OnRmvGroupSuccess(const std::string& grpId);
    virtual void OnRmvGroupFailed(const std::string& grpId, const std::string& reason, int code);

    // from MSClientDelegate
    virtual void OnMsgServerConnected();
    virtual void OnMsgServerConnecting();
    virtual void OnMsgServerDisconnect();
    virtual void OnMsgServerConnectionFailure();
    virtual void OnMsgClientInitializing();
    virtual void OnMsgClientInitialized();
    virtual void OnMsgClientUnInitialize();
    virtual void OnMsgClientInitializeFailure();

    // from MSSubMessageDelegate
    virtual void OnSendMessage(const std::string& msgId, int code);
    virtual void OnRecvTxtMessage(MSMessage* txtMsg);
    virtual void OnRecvSelfDefMessage(MSMessage* sdefMsg);
    virtual void OnNotifyLiveMessage(MSMessage* livMsg);
    virtual void OnNotifyRedEnvelopeMessage(MSMessage* renMsg);
    virtual void OnNotifyBlacklistMessage(MSMessage* blkMsg);
    virtual void OnNotifyForbiddenMessage(MSMessage* fbdMsg);
    virtual void OnNotifySettedMgrMessage(MSMessage* mgrMsg);
    virtual void OnNotifyOtherLogin(int code);


public:
    void Init(const std::string& strGroupId, const std::string& strUserId, const std::string& strIp, int port);
    void Unin();

    int AddGroup(const std::string& groupid);
    int SendGroupMsg(const std::string& groupid, const std::string& msg);
private:
    MSGroupManager*         mGroupManager;
    MSClientManager*        mClientManager;
    MSMessageManager*       mMessageManager;

    // log file
    FILE*                   mSendLog;
    FILE*                   mRecvLog;

    std::string             mGroupId;
    std::string             mUserId;
    std::string             mToken;
    std::string             mNname;

};

#endif /* defined(__SeqnClient__ClientSession__) */
