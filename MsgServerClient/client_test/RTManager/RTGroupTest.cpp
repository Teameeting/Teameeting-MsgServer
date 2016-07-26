//
//  RTGroupTest.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTGroupTest.hpp"

extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
}

const int kStop = 1;
const int kRunOnce = 2;
const int kRunSyncSeqn = 3;
const int kRunSyncData = 4;
const int kPostDelay = 3000;

RTGroupTest::RTGroupTest()
: rtc::MessageHandler()
, rtc::Thread(NULL)
, mIsRun(false)
, mThreadRun(false)
, mTimes(0)
, mMsgClient(NULL)
{

}

RTGroupTest::~RTGroupTest()
{
    if (mMsgClient)
    {
        mMsgClient->Unin();
         delete mMsgClient;
         mMsgClient = NULL;
    }
    if (mThreadRun)
        this->Stop();
}

void RTGroupTest::RunTest(int flag, const std::string& name)
{
    mIsRun = true;
    mName = name;
    mMsgClient = new RTMsgClient(mName);
    mMsgClient->SetPData(this);
    mMsgClient->SetDataCallback(&RTGroupTest::ClientDataCallbackStatic);
    mMsgClient->GrpInitSync();
    this->Start();
    mThreadRun = true;
    if (flag == kRunOnce) {
        PostDelayed(kPostDelay, this, kRunOnce);
    }
    while (mIsRun) {
        rtc::Thread::SleepMs(1000);
    }
    printf("RTGroupTest::RunTest finished...\n");
}

void RTGroupTest::OnMessage(rtc::Message* msg)
{
    if (msg->message_id == kStop) {
        mIsRun = false;
    } else if (msg->message_id == kRunOnce) {
        RunOnce();
        //PostDelayed(kPostDelay, this, kStop);
    } else if (msg->message_id == kRunSyncSeqn) {
        RunSyncSeqn();
    } else if (msg->message_id == kRunSyncData) {
        RunSyncData();
    }
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void RTGroupTest::ClientDataCallbackStatic(void* pd, int data)
{
    RTGroupTest* pc = (RTGroupTest*)pd;
    if (pc)
    {
        pc->ClientDataCallback(data);
    }
    else
    {
        printf("%s pc is null, line:%d\n", __FUNCTION__, __LINE__);
    }
}

void RTGroupTest::ClientDataCallback(int data)
{
    if (data==1)
    {
        printf("ClientDataCallback was called, server sync seqn\n");
        PostDelayed(kPostDelay, this, kRunSyncSeqn);
    } else if (data==2)
    {
        printf("ClientDataCallback was called, server sync data\n");
        PostDelayed(kPostDelay, this, kRunSyncData);
        PostDelayed(kPostDelay, this, kRunSyncSeqn);
    } else {
        printf("ClientDataCallback was called, data :%d not handle\n\n", data);
    }
}

void RTGroupTest::RunSyncSeqn()
{
    printf("RTGroupTest::RunSyncSeqn was called\n");
}

void RTGroupTest::RunSyncData()
{
    printf("RTGroupTest::RunSyncData was called\n");
}

void RTGroupTest::RunCreateGroup()
{
    printf("RTGroupTest::RunCreateGroup was called\n");
    std::string groupid("wocaowocaowocao");
    mMsgClient->CreateGroupSeqn(groupid);
    std::string groupid2("9008000036");
    mMsgClient->CreateGroupSeqn(groupid2);
    std::string groupid3("9008000015");
    mMsgClient->CreateGroupSeqn(groupid3);
}

void RTGroupTest::RunDeleteGroup()
{
    printf("RTGroupTest::RunDeleteGroup was called\n");
    std::string groupid("wocaowocaowocao");
    mMsgClient->DeleteGroupSeqn(groupid);
}


void RTGroupTest::RunOnce()
{
    //TODO:
    RunCreateGroup();
    //RunDeleteGroup();
}

