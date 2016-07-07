//
//  RTMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/24/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "RTMsgClient.hpp"
#include "webrtc/base/logging.h"

#include "../../client_common/proto/common_msg.pb.h"
#include "../../client_common/proto/sys_msg_type.pb.h"
#include "../../client_common/proto/storage_msg_type.pb.h"
#include "../../client_common/proto/entity_msg_type.pb.h"

#define DEF_PROTO 1

RTMsgClient::RTMsgClient(const std::string& uid)
: mMsgClient()
, mGrpMsgClient()
, mHttpClient()
, mUserid(uid)
, mAuth("")
, mUname("")
, mPushToken("")
, mMsgServer("127.0.0.1")
, mCurRoomId("4008696859")
, mMsgPort(6630)
, mIsOnline(false)
, mRecvNum(0)
, mConnNum(0)
, mDisconnNum(0)
, mConnFailNum(0)
, mCond(PTHREAD_COND_INITIALIZER)
, mMutex(PTHREAD_MUTEX_INITIALIZER)
, mPData(NULL)
, mDataCallback(NULL)
{

}

RTMsgClient::~RTMsgClient()
{

}


void RTMsgClient::OnSndMsg(int code, const std::string& msg)
{

}


void RTMsgClient::OnCmdGroup(int code, int cmd, const std::string& groupid, const MSCbData& data)
{

    LOG(INFO) << __FUNCTION__ << " was called";
}


void RTMsgClient::OnRecvMsg(int64 seqn, const std::string& msg)
{
    LOG(INFO) << __FUNCTION__ << " was called";
}

void RTMsgClient::OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg)
{
    LOG(INFO) << __FUNCTION__ << " was called";

}

void RTMsgClient::OnSyncSeqn(int64 seqn, int role)
{
    LOG(INFO) << __FUNCTION__ << " was called";

}

void RTMsgClient::OnSyncGroupSeqn(const std::string& groupid, int64 seqn)
{
    LOG(INFO) << __FUNCTION__ << " was called";

}

void RTMsgClient::OnGroupNotify(int code, const std::string& seqnid)
{
    LOG(INFO) << __FUNCTION__ << " was called";

}

void RTMsgClient::OnNotifySeqn(int code, const std::string& seqnid)
{
    LOG(INFO) << __FUNCTION__ << " was called";

}

void RTMsgClient::OnNotifyData(int code, const std::string& seqnid)
{
    LOG(INFO) << __FUNCTION__ << " was called";

}

void RTMsgClient::OnMsgServerConnected()
{
    LOG(INFO) << __FUNCTION__ << " was called";
    mConnNum++;
    mIsOnline = true;
}

void RTMsgClient::OnMsgServerConnecting()
{

}

void RTMsgClient::OnMsgServerDisconnect()
{
    LOG(INFO) << __FUNCTION__ << " was called";
    mDisconnNum++;
    mIsOnline = false;
}

void RTMsgClient::OnMsgServerConnectionFailure()
{
    LOG(INFO) << __FUNCTION__ << " was called";
    mConnFailNum++;
    mIsOnline = false;
}



int RTMsgClient::Register()
{
    if (mHttpClient.UserInit(mUserid)==0) {
        mUserid = mHttpClient.GetUserid();
        mAuth = mHttpClient.GetAuth();
        mUname = mHttpClient.GetUname();
        mPushToken = mHttpClient.GetPushToken();
        return 0;
    } else {
        LOG(INFO) << __FUNCTION__ << " error";
        return -1;
    }
}

int RTMsgClient::ApplyRoom()
{
#if 1
    if (mHttpClient.ApplyRoom()==0) {
        mHttpClient.SyncRoomSet(mRoomSet);
        mCurRoomId = (*mRoomSet.begin());
        printf("RTMsgClient mRoomSet.size:%ld\n", mRoomSet.size());
        for (auto lit=mRoomSet.begin(); lit!=mRoomSet.end(); ++lit) {
            printf("RTMsgClient::ApplyRoom RoomSet roomid:%s\n", (*lit).c_str());
        }
        return 0;
    } else {
        LOG(INFO) << __FUNCTION__ << " error";
        return -1;
    }
#endif
}

void RTMsgClient::Init(int module)
{
    mMsgClient.Init(mUserid, mAuth, mUname, module);
    mMsgClient.RegisterMsgCb(this);
    mMsgClient.ConnToServer(mMsgServer, mMsgPort);
}

void RTMsgClient::Unin()
{
    mMsgClient.UnRegisterMsgCb(nullptr);
    mMsgClient.Unin();
}

bool RTMsgClient::Connecting()
{
    while(mMsgClient.MSStatus()!=MSState::MSCONNECTED)sleep(1);
    return true;
}

void RTMsgClient::SendMsg(const std::string& msg)
{
    if (mIsOnline) {
        std::string msgid;
        mMsgClient.SndMsg(msgid, mCurRoomId, "RoomName", msg, pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, pms::EModuleType::TLIVE, pms::EMsgFlag::FSINGLE);
    }
}

void RTMsgClient::InitSync()
{
    Init(pms::EModuleType::TLIVE);
    Connecting();
    mIsOnline = true;
}

void RTMsgClient::SyncSeqn()
{
    if (mIsOnline) {
        printf("RTMsgClient::SyncSeqn was called not implement\n");
        //mMsgClient.SyncSeqn();
    }
    else
    {
         printf("RTMsgClient::SyncSeqn mIsOnline is false\n");
    }
}

void RTMsgClient::SyncData()
{
    if (mIsOnline) {
        printf("RTMsgClient::SyncData was called not implement\n");
        //mMsgClient.SyncData();
    }
    else
    {
         printf("RTMsgClient::SyncSeqn mIsOnline is false\n");
    }
}

void RTMsgClient::SendMessage(const std::string& msg)
{
    if (mIsOnline) {
        printf("RTMsgClient::SyncData was called\n");
        std::string msgid;
        mMsgClient.SndMsg(msgid, mCurRoomId, "RoomName", msg, pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, pms::EModuleType::TLIVE, pms::EMsgFlag::FSINGLE);
    }
    else
    {
         printf("RTMsgClient::SyncSeqn mIsOnline is false\n");
    }
}

void RTMsgClient::SendMessageTo(const std::string& msg, const std::string& name)
{
    if (mIsOnline) {
        printf("RTMsgClient::SyncData was called\n");
        std::vector<std::string> v;
        v.push_back(name);
        std::string msgid;
        mMsgClient.SndMsgTo(msgid, mCurRoomId, "RoomName", msg, pms::EMsgTag::TCHAT, pms::EMsgType::TTXT, pms::EModuleType::TLIVE, pms::EMsgFlag::FSINGLE, v);
    }
    else
    {
         printf("RTMsgClient::SyncSeqn mIsOnline is false\n");
    }
}

void RTMsgClient::AddGroup()
{
    if (mIsOnline) {
        printf("RTMsgClient::AddGroup was called\n");
        mMsgClient.AddGroup(mCurRoomId);
    }
        printf("RTMsgClient::AddGroup was called~~\n");
}

void RTMsgClient::GrpInit(int module)
{
    mGrpMsgClient.Init(mUserid, mAuth, module);
    mGrpMsgClient.RegisterMsgCb(this);
    mGrpMsgClient.ConnToServer("192.168.7.207", 6690);
}

void RTMsgClient::GrpUnin()
{
    mGrpMsgClient.UnRegisterMsgCb(nullptr);
    mGrpMsgClient.Unin();
}

bool RTMsgClient::GrpConnecting()
{
    printf("RTMsgClient::GrpConnecting\n");
    while(mGrpMsgClient.MSStatus()!=MSState::MSCONNECTED)sleep(1);
    printf("RTMsgClient::GrpConnecting ok\n");
    mIsOnline = true;
    return true;
}

void RTMsgClient::GrpInitSync()
{
    GrpInit(pms::EModuleType::TLIVE);
    GrpConnecting();
}

void RTMsgClient::GrpSyncGroupData(const std::string& userid, const std::string groupid, int64 curseqn)
{
    if (mIsOnline) {
        mMsgClient.SyncGroupData(groupid, curseqn);
    }
}

void RTMsgClient::CreateGroupSeqn()
{
    if (mIsOnline) {
        printf("RTMsgClient::CreateGroupSeqn was called not implement\n");
        //mMsgClient.CreateGroupSeqn("9a4f3730-f643-422a-a3a1-eae557060a90", "hahaha");
    }
}

void RTMsgClient::DeleteGroupSeqn()
{
    if (mIsOnline) {
        printf("RTMsgClient::DeleteGroupSeqn was called not implement\n");
    }
}


void RTMsgClient::ShowRecvMsg()
{
    printf("User: %s ShowRecvMsg num:%d\n", mUserid.c_str(), mRecvNum);
    //for (auto& x : mMsgList) {
    //    printf("SHowRecvMsg :%s\n", x.c_str());
    //}
}
