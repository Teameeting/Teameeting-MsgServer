//
//  ClientSession.cpp
//  SeqnClient
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "ClientSession.h"
#include "ClientManager.h"
#include "RTUtils.hpp"
#include "rtklog.h"

ClientSession::ClientSession()
: mGroupManager(nullptr)
, mMessageManager(nullptr)
, mClientManager(nullptr) {
}

ClientSession::~ClientSession()
{
}

// from MSGroupDelegate
void ClientSession::OnAddGroupSuccess(const std::string& grpId)
{
    LI("%s was called\n", __FUNCTION__);
}

void ClientSession::OnAddGroupFailed(const std::string& grpId, const std::string& reason, int code)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnRmvGroupSuccess(const std::string& grpId)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnRmvGroupFailed(const std::string& grpId, const std::string& reason, int code)
{
    LI("%s was called\n", __FUNCTION__);

}

// from MSClientDelegate
void ClientSession::OnMsgServerConnected()
{
    LI("%s was called\n", __FUNCTION__);
    const char* groupid = "TestGroupId01";
    ClientManager::Instance().AddGroup(groupid);

}

void ClientSession::OnMsgServerConnecting()
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnMsgServerDisconnect()
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnMsgServerConnectionFailure()
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnMsgClientInitializing()
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnMsgClientInitialized()
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnMsgClientUnInitialize()
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnMsgClientInitializeFailure()
{
    LI("%s was called\n", __FUNCTION__);

}

// from MSSubMessageDelegate
void ClientSession::OnSendMessage(const std::string& msgId, int code)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnRecvTxtMessage(MSMessage* txtMsg)
{
    LI("%s was called, msg:%s\n", __FUNCTION__, txtMsg->GetContent().c_str());

}

void ClientSession::OnRecvSelfDefMessage(MSMessage* sdefMsg)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnNotifyLiveMessage(MSMessage* livMsg)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnNotifyRedEnvelopeMessage(MSMessage* renMsg)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnNotifyBlacklistMessage(MSMessage* blkMsg)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnNotifyForbiddenMessage(MSMessage* fbdMsg)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnNotifySettedMgrMessage(MSMessage* mgrMsg)
{
    LI("%s was called\n", __FUNCTION__);

}

void ClientSession::OnNotifyOtherLogin(int code)
{
    LI("%s was called\n", __FUNCTION__);

}

// public
void ClientSession::Init(const std::string& strGroupId, const std::string& strUserId, const std::string& strIp, int nPort)
{
    mGroupManager = new MSGroupManager();
    mClientManager = new MSClientManager();
    mMessageManager = new MSMessageManager();
    mGroupManager->AddDelegate(this);
    mClientManager->AddDelegate(this);
    mMessageManager->AddDelegate(this);

    std::string userid = strUserId;
    std::string token = "token";
    std::string nname = "nname";
    std::string roomid = strGroupId;

    mClientManager->SetNickName(nname);
    mClientManager->SetToken(token);
    mClientManager->SetUIconUrl("hehehe");
    int ret = mClientManager->InitMsgClient(userid, token, nname);
    if (ret != 0)
    {
         printf("ClientSession::Init InitMsgClient failed, ret:%d\n", ret);
         exit(0);
    }

    mClientManager->ConnToServer(strIp, nPort);
}

void ClientSession::Unin()
{
    if (mMessageManager)
    {
        mMessageManager->DelDelegate(nullptr);
        delete mMessageManager;
        mMessageManager = nullptr;
    }
    if (mGroupManager)
    {
        mGroupManager->DelDelegate(nullptr);
        delete mGroupManager;
        mGroupManager = nullptr;
    }
    if (mClientManager)
    {
        mClientManager->DelDelegate(nullptr);
        mClientManager->UninMsgClient();
        delete mClientManager;
        mClientManager = nullptr;
    }
}

int ClientSession::AddGroup(const std::string& groupid)
{
    mGroupManager->AddGroup(groupid);
    return 0;
}

int ClientSession::SendGroupMsg(const std::string& groupid, const std::string& msg)
{
    MSTxtMessage *txtMsg = new MSTxtMessage();
    txtMsg->SetGroupId(groupid);
    txtMsg->SetContent(msg);
    txtMsg->SetPush(0);
    const char* msgid = mMessageManager->SendTxtMsg(txtMsg);
    if (!msgid) return -1;
    LI("ClientSession::SendGroupMsg msgid:%s\n", msgid);
    if (msgid)
    {
         free((void*)msgid);
         msgid = nullptr;
    }
    return 0;
}
