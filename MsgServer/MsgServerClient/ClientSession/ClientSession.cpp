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

static long s_send_msg_times = 0;
static long s_recv_msg_times = 0;

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
    char buf[256] = {0};
    sprintf(buf, "Grp:%s:Usr:%s:%ld\n", mGroupId.c_str(), mUserId.c_str(), s_recv_msg_times++);
    fwrite(buf, 1, strlen(buf), mRecvLog);
    fflush(mRecvLog);

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

    mUserId = strUserId;
    mToken = "token";
    mNname = "nname";
    mGroupId = strGroupId;

    mClientManager->SetNickName(mNname);
    mClientManager->SetToken(mToken);
    mClientManager->SetUIconUrl("hehehe");
    int ret = mClientManager->InitMsgClient(mUserId, mToken, mNname);
    if (ret != 0)
    {
         printf("ClientSession::Init InitMsgClient failed, ret:%d\n", ret);
         exit(0);
    }

    mClientManager->ConnToServer(strIp, nPort);

    std::string sendLogPath("./client_logs/Send_");
    sendLogPath.append(strGroupId).append("_").append(strUserId);

    std::string recvLogPath("./client_logs/Recv_");
    recvLogPath.append(strGroupId).append("_").append(strUserId);

    mSendLog = fopen(sendLogPath.c_str(), "w+");
    mRecvLog = fopen(recvLogPath.c_str(), "w+");
    if (!mSendLog || !mRecvLog)
    {
        printf("ClientSession::Init fopen file failed\n");
         exit(0);
    }
}

void ClientSession::Unin()
{
    if (mRecvLog)
    {
         fclose(mRecvLog);
         mRecvLog = nullptr;
    }
    if (mSendLog)
    {
        fclose(mSendLog);
        mSendLog = nullptr;
    }
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

    char buf[256] = {0};
    sprintf(buf, "Grp:%s:Usr:%s:%ld\n", mGroupId.c_str(), mUserId.c_str(), s_send_msg_times++);
    LI("ClientSession::SendGroupMsg write log msg:%s\n", buf);
    size_t n = fwrite(buf, 1, strlen(buf), mSendLog);
    fflush(mSendLog);
    printf("ClientSession::SendGroupMsg fwrite num:%u, strlen(buf):%u\n, ", n, strlen(buf));

    if (msgid)
    {
         free((void*)msgid);
         msgid = nullptr;
    }
    return 0;
}
