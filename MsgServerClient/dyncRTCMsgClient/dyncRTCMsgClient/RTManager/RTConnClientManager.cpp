//
//  RTConnClientManager.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTConnClientManager.hpp"
#include "RTMsgRoom.hpp"
#include "RTMsgThreadPool.hpp"

const int kStop = 0;
const int kRunRun = 1;
const int kRunOnce = 2;
const int kRunThread = 3;
const int kRunApplyRoom = 4;
const int kRunLoginout = 5;
const int kRunSendMsg = 6;

const int kPostDelay = 3000;

const int kTimes = 3;

RTConnClientManager::RTConnClientManager()
: rtc::MessageHandler()
, rtc::Thread(NULL)
, mIsRun(false)
, mThreadRun(false)
, mTimes(0){

}

RTConnClientManager::~RTConnClientManager()
{
    if (mThreadRun)
        this->Stop();
}

void RTConnClientManager::RunTest(int flag)
{
    mIsRun = true;
    this->Start();
    mThreadRun = true;
    if (flag == kRunRun) {
        PostDelayed(kPostDelay, this, kRunRun);
    } else if (flag == kRunOnce) {
        PostDelayed(kPostDelay, this, kRunOnce);
    } else if (flag == kRunThread) {
        PostDelayed(kPostDelay, this, kRunThread);
    } else if (flag == kRunApplyRoom) {
        PostDelayed(kPostDelay, this, kRunApplyRoom);
    } else if (flag == kRunLoginout) {
        PostDelayed(kPostDelay, this, kRunLoginout);
    } else if (flag == kRunSendMsg) {
         PostDelayed(kPostDelay, this, kRunSendMsg);
    }
    while (mIsRun) {
        rtc::Thread::SleepMs(1000);
    }
    printf("RTConnClientManager::RunTest finished...\n");
}

void RTConnClientManager::OnMessage(rtc::Message* msg)
{
    printf("RTConnClientManager::OnMessage msg_id:%u\n", msg->message_id);
    if (msg->message_id == kStop) {
        mIsRun = false;
    } else if (msg->message_id == kRunRun) {
        RunRun();
        PostDelayed(kPostDelay, this, kStop);
    } else if (msg->message_id == kRunOnce) {
        RunOnce();
        PostDelayed(kPostDelay, this, kStop);
    } else if (msg->message_id == kRunThread) {
        mTimes++;
        printf("\n\n########################Begin TestTimes:%d#####################\n\n", mTimes);
        RunThread();
        printf("\n\n########################End TestTimes:%d#####################\n\n", mTimes);
        if (mTimes==kTimes) {
            //PostDelayed(kPostDelay, this, kStop);
            PostDelayed(kPostDelay, this, kRunThread);
        } else {
            PostDelayed(kPostDelay, this, kRunThread);
        }
    } else if (msg->message_id == kRunApplyRoom) {
        RunApplyRoom();
        PostDelayed(kPostDelay, this, kStop);
    } else if (msg->message_id == kRunLoginout) {
        RunLoginout();
        PostDelayed(kPostDelay, this, kStop);
    } else if (msg->message_id == kRunSendMsg) {
         RunSendMsg();
         PostDelayed(kPostDelay, this, kStop);
    }
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void RTConnClientManager::RunRun()
{
    RTMsgThreadManager::Instance()->RunForTest(kRunRun);
}

void RTConnClientManager::RunOnce()
{
    RTMsgThreadManager::Instance()->RunForTest(kRunOnce);
}

void RTConnClientManager::RunThread()
{
    RTMsgThreadManager::Instance()->RunForTest(kRunThread);
}

void RTConnClientManager::RunApplyRoom()
{
    RTMsgThreadManager::Instance()->RunForTest(kRunApplyRoom);
}

void RTConnClientManager::RunLoginout()
{
    RTMsgThreadManager::Instance()->RunForTest(kRunLoginout);
}

void RTConnClientManager::RunSendMsg()
{
    RTMsgThreadManager::Instance()->RunForTest(kRunSendMsg);
}
