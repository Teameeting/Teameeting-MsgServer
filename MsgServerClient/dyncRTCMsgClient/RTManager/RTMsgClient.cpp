//
//  RTMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/24/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include "RTMsgClient.hpp"
#include "webrtc/base/logging.h"
#include "proto/common_msg.pb.h"
#include "proto/meet_msg.pb.h"
#include "proto/meet_msg_type.pb.h"

#define DEF_PROTO 1

RTMsgClient::RTMsgClient(const std::string& uid)
: mMsgClient()
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
, mMutex(PTHREAD_MUTEX_INITIALIZER){

}

RTMsgClient::~RTMsgClient()
{

}

void RTMsgClient::OnSndMsg(const std::string& msg)
{
#if DEF_PROTO
    pms::MeetMsg meet;
    meet.ParseFromString(msg);
    LOG(INFO) << "RTMsgClient::OnSndMsg--->";
    switch(meet.msg_tag()) {
        case pms::EMsgTag::TCHAT:
            {
                LOG(INFO) <<  mUserid + " recv num:" << mRecvNum << " EMsgTag::TCHAT: " << meet.msg_cont();
                mRecvNum++;
                //mMsgList.push_back(meet.msg_cont());
            }
            break;
        case pms::EMsgTag::TENTER:
            {
                LOG(INFO) << "EMsgTag::TENTER " << meet.msg_cont();
            }
            break;
        case pms::EMsgTag::TLEAVE:
            {
                LOG(INFO) << "EMsgTag::TLEAVE " << meet.msg_cont();
            }
            break;
        case pms::EMsgTag::TNOTIFY:
            {
                LOG(INFO) << "EMsgTag::TNOTIFY " << meet.msg_cont();
            }
        default:
            {

            }
            break;
    }
#else
    LOG(INFO) << "not define DEF_PROTO";
#endif
}

void RTMsgClient::OnGetMsg(const std::string& msg)
{
    //LOG(INFO) << __FUNCTION__ << " msg:" << msg;
}

void RTMsgClient::OnMsgServerConnected()
{
    LOG(INFO) << __FUNCTION__ << " was called";
    mConnNum++;
    mIsOnline = true;
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

void RTMsgClient::OnMsgServerState(MSState state)
{
    //LOG(INFO) << __FUNCTION__ << " was called state:" << state;
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
    mMsgClient.Init(this, mUserid, mAuth, mUname, module, mMsgServer, mMsgPort);
}

void RTMsgClient::Unin()
{
    mMsgClient.Unin();
}

bool RTMsgClient::Connecting()
{
    while(mMsgClient.MSStatus()!=MSState::MSCONNECTED)sleep(1);
    return true;
}

void RTMsgClient::EnterRoom()
{
    if (mIsOnline) {
        mMsgClient.OptRoom(pms::EMsgTag::TENTER, mCurRoomId, "RoomName", "");
    }
}

void RTMsgClient::LeaveRoom()
{
    if (mIsOnline) {
        mMsgClient.OptRoom(pms::EMsgTag::TLEAVE, mCurRoomId, "RoomName", "");
    }
}

void RTMsgClient::SendMsg(const std::string& msg)
{
    if (mIsOnline) {
        mMsgClient.SndMsg(mCurRoomId, "RoomName", msg);
    }
}

void RTMsgClient::InitSync()
{
    mIsOnline = true;
    Init(pms::EModuleType::TLIVE);
    Connecting();
}

void RTMsgClient::SyncSeqn()
{
    mIsOnline = true;
    if (mIsOnline) {
        printf("RTMsgClient::SyncSeqn was called\n");
        mMsgClient.SyncSeqn();
    }
}

void RTMsgClient::SyncData()
{
    mIsOnline = true;
    if (mIsOnline) {
        printf("RTMsgClient::SyncData was called\n");
        mMsgClient.SyncData();
    }
}

void RTMsgClient::SendMessage(const std::string& msg)
{
    mIsOnline = true;
    if (mIsOnline) {
        printf("RTMsgClient::SyncData was called\n");
        mMsgClient.SndMsg(mCurRoomId, "RoomName", msg);
    }
}

void RTMsgClient::ShowRecvMsg()
{
    printf("User: %s ShowRecvMsg num:%d\n", mUserid.c_str(), mRecvNum);
    //for (auto& x : mMsgList) {
    //    printf("SHowRecvMsg :%s\n", x.c_str());
    //}
}