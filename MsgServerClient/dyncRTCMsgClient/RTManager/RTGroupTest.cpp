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
    if (mMsgClient)
        mMsgClient->SyncSeqn();
}

void RTGroupTest::RunSyncData()
{
    if (mMsgClient)
        mMsgClient->SyncData();
}

void RTGroupTest::RunOnce()
{
    //TODO:
    ////TestSendGroup();
    //TestSendOnce();
    //TestSendLoop();
}

void RTGroupTest::TestSendGroup()
{
    if (mName.compare("xddxdd")==0)
    {
        //while(1)
        //    rtc::Thread::SleepMs(1000);
    } else {
        {
            std::cout << "TestSendOnce name:" << mName << std::endl;
            mMsgClient->GrpGenNotify();

            rtc::Thread::SleepMs(500);
        }
    }
}


void RTGroupTest::TestSendOnce()
{
    if (mName.compare("xddxdd")==0)
    {
        //while(1)
        //    rtc::Thread::SleepMs(1000);
    } else {
        long long i = 0;
        {
            char ii[32] = {0};
            std::cout << "TestSendOnce name:" << mName << std::endl;
            sprintf(ii, "hello world%lld\n", i++);
            std::string m(ii);
            std::string n("xddxdd");
            mMsgClient->SendMessageTo(m, n);
            rtc::Thread::SleepMs(500);
        }
    }
}

void RTGroupTest::TestSendLoop()
{
    if (mName.compare("xddxdd")==0)
    {
        //while(1)
        //    rtc::Thread::SleepMs(1000);
    } else {
        long long i = 0;
        //int t = 5;
        //while(t--)
        while(1)
        {
            char ii[16] = {0};
            std::cout << "TestSendOnce name:" << mName << std::endl;
            sprintf(ii, "hello world%lld", i++);
            std::string m(ii);
            std::string n("xddxdd");
            mMsgClient->SendMessageTo(m, n);
            rtc::Thread::SleepMs(200);
        }
    }
}
