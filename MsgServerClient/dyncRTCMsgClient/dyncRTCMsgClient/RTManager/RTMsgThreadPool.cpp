//
//  RTMsgThreadPool.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/25/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTMsgThreadPool.hpp"
#include <ostream>
#include <string>
#include <sstream>

const int kMessageIdStop = 0;
const int kMessageIdRun = 1;
const int kMessageIdOnce = 2;
const int kMessageIdThread = 3;
const int kMessageIdApplyRoom = 4;
const int kMessageIdLoginout = 5;

const int kDelayTime = 1000;

const int kTestNumber = 10;

RTMsgThread::RTMsgThread()
: rtc::Thread(NULL){

}

RTMsgThread::~RTMsgThread()
{

}

void RTMsgThread::OnMessage(rtc::Message* msg)
{
    printf("RTMsgThread::OnMessage message_id:%u, thread name:%s\n", msg->message_id, this->name().c_str());
    MsgData *md = (MsgData*)msg->pdata;
    if (msg->message_id == kMessageIdRun) {
        MessageRun(md->data());
    } else if (msg->message_id == kMessageIdOnce) {
        MessageOnce(md->data());
    } else if (msg->message_id == kMessageIdThread) {
        MessageThread(md->data());
    } else if (msg->message_id == kMessageIdApplyRoom) {
        MessageApplyRoom(md->data());
    } else if (msg->message_id == kMessageIdLoginout) {
        MessageLoginout(md->data());
    }
}

void RTMsgThread::MessageRun(RoomInfo& info)
{
    std::string rid = "400000000807";
    RTMsgRoom room(1, rid);
    room.RunRun();
    RTMsgThreadManager::Instance()->NotifyStopThread();
}

void RTMsgThread::MessageOnce(RoomInfo& info)
{
    std::string rid = "400000000807";
    RTMsgRoom room(1, rid);
    room.RunOnce();
    RTMsgThreadManager::Instance()->NotifyStopThread();
}

void RTMsgThread::MessageThread(RoomInfo& info)
{
    RTMsgRoom room(info._roomNum, info._roomId);
    room.RunThread();
    RTMsgThreadManager::Instance()->NotifyStopThread();
}

void RTMsgThread::MessageApplyRoom(RoomInfo& info)
{
    std::string rid = "400000000807";
    RTMsgRoom room(1, rid);
    room.RunApplyRoom();
    RTMsgThreadManager::Instance()->NotifyStopThread();
}

void RTMsgThread::MessageLoginout(RoomInfo& info)
{
    RTMsgRoom room(info._roomNum, info._roomId);
    room.RunLoginout();
    RTMsgThreadManager::Instance()->NotifyStopThread();
}

///////////////////////////////////////////////////////
///////////////////////////////////////////////////////
///////////////////////////////////////////////////////

void RTMsgThreadManager::OnMessage(rtc::Message *msg)
{
    if (msg->message_id == kMessageIdStop) {
        this->Stop();
        mIsRun = false;
    }
#if 0
    if (msg->message_id == kMessageIdRun) {
        AddThread(1);
        RunThread(kMessageIdRun);
        PostDelayed(kDelayTime, this, kMessageIdStop);
    } else if (msg->message_id == kMessageIdOnce) {
        AddThread(1);
        RunThread(kMessageIdOnce);
        PostDelayed(kDelayTime, this, kMessageIdStop);
    } else if (msg->message_id == kMessageIdThread) {
        AddThread(kTestNumber);
        RunThread(kMessageIdThread);
        PostDelayed(kDelayTime, this, kMessageIdStop);
    } else if (msg->message_id == kMessageIdApplyRoom) {
        AddThread(1);
        RunThread(kMessageIdApplyRoom);
        PostDelayed(kDelayTime, this, kMessageIdStop);
    } else if (msg->message_id == kMessageIdLoginout) {
        AddThread(kTestNumber);
        RunThread(kMessageIdLoginout);
        PostDelayed(kDelayTime, this, kMessageIdStop);
    }
#endif
}

void RTMsgThreadManager::AddThread(int num)
{
    for (int i=0; i<num; i++) {
        RTMsgThread *t = new RTMsgThread();
        t->SetName("Thread--", t);
        mThreadPoolSet.insert(t);
    }
    for (auto& x : mThreadPoolSet) {
        x->Start();
    }
    mTestThreadNum = num;
    printf("AddThread size:%ld\n", mThreadPoolSet.size());
}

void RTMsgThreadManager::DelThread()
{
    for (auto& x : mThreadPoolSet) {
        x->Stop();
    }
    for (auto& x : mThreadPoolSet) {
        delete x;
    }
    mThreadPoolSet.clear();
    printf("DelThread size:%ld\n", mThreadPoolSet.size());
}

void RTMsgThreadManager::RunThread(int flag)
{
    int i=0;
    GetRoomIds();
    if (mRoomIds.size()==0) {
        printf("RunThread mRoomIds.size is 0, so return\n");
        return;
    }
    //mRoomIds.push_back("400000000802");
    for (auto& x : mThreadPoolSet) {
        //x->PostDelayed(kDelay, x);
        //x->PostDelayed(kDelay, x, 100);
        std::ostringstream oss("");
        oss << i;
        RoomInfo info;
        info._roomId = mRoomIds[i];
        info._roomNum = i;
        ++i;
        MsgData md(info);
        if (flag==kMessageIdRun) {
            x->Post(x, kMessageIdRun, &md);
        } else if (flag==kMessageIdOnce) {
            x->Post(x, kMessageIdOnce, &md);
        } else if (flag==kMessageIdThread) {
            x->Post(x, kMessageIdThread, &md);
        } else if (flag==kMessageIdApplyRoom) {
            x->Post(x, kMessageIdApplyRoom, &md);
        } else if (flag==kMessageIdLoginout) {
            x->Post(x, kMessageIdLoginout, &md);
        }
        rtc::Thread::SleepMs(100);
    }
    rtc::Thread::SleepMs(1000);
}

void RTMsgThreadManager::RunThread(int flag, RoomInfo &info)
{
    
}

void RTMsgThreadManager::RunTest(int flag)
{
    mIsRun = true;
    this->SetName("Thread--", this);
    this->Start();
    if (flag == kMessageIdRun) {
        AddThread(1);
        RunThread(kMessageIdRun);
    } else if (flag == kMessageIdOnce) {
        AddThread(1);
        RunThread(kMessageIdOnce);
        this->Post(this, kMessageIdOnce, NULL);
    } else if (flag == kMessageIdThread) {
        AddThread(kTestNumber);
        RunThread(kMessageIdThread);
    } else if (flag == kMessageIdApplyRoom) {
        AddThread(1);
        RunThread(kMessageIdApplyRoom);
    } else if (flag == kMessageIdLoginout) {
        AddThread(kTestNumber);
        RunThread(kMessageIdLoginout);
    }
    while (mIsRun) {
        rtc::Thread::SleepMs(1000);
    }
    DelThread();
}

void RTMsgThreadManager::GetRoomIds()
{
    FILE* fRoomIds = fopen("test_roomids", "r");
    if (fRoomIds==NULL) {
        std::cout << "fopen file test_roomids error" << std::endl;
        return;
    }
    mRoomIds.clear();
    char ids[16] = {0};
    while(!feof(fRoomIds) && fgets(ids, 16, fRoomIds)) {
        ids[12] = '\0';
        mRoomIds.push_back(ids);
        memset(ids, 0, 16);
    }
    printf("RTMsgThreadManager::GetRoomIds mRoomId.size:%ld\n", mRoomIds.size());
}

void RTMsgThreadManager::ShowRoomIds()
{
    for (RoomIdSetIt it=mRoomIds.begin(); it!=mRoomIds.end(); ++it) {
        printf("RTMsgThreadManager::ShowRoomIds:%s\n", (*it).c_str());
    }
}

void RTMsgThreadManager::NotifyStopThread()
{
    mIsRun = false;
    this->Stop();
}