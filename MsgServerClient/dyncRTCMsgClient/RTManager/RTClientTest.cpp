//
//  RTClientTest.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTClientTest.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const int kStop = 1;
const int kRunOnce = 2;
const int kRunSyncSeqn = 3;
const int kRunSyncData = 4;
const int kPostDelay = 3000;

RTClientTest::RTClientTest()
: rtc::MessageHandler()
, rtc::Thread(NULL)
, mIsRun(false)
, mThreadRun(false)
, mTimes(0)
, mMsgClient(NULL)
{

}

RTClientTest::~RTClientTest()
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

void RTClientTest::RunTest(int flag, const std::string& name)
{
    mIsRun = true;
    mName = name;
    mMsgClient = new RTMsgClient(mName);
    mMsgClient->SetPData(this);
    mMsgClient->SetDataCallback(&RTClientTest::ClientDataCallbackStatic);
    mMsgClient->InitSync();
    this->Start();
    mThreadRun = true;
    if (flag == kRunOnce) {
        PostDelayed(kPostDelay, this, kRunOnce);
    }
    while (mIsRun) {
        rtc::Thread::SleepMs(1000);
    }
    printf("RTClientTest::RunTest finished...\n");
}

void RTClientTest::OnMessage(rtc::Message* msg)
{
    printf("RTClientTest::OnMessage msg_id:%u\n", msg->message_id);
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

void RTClientTest::ClientDataCallbackStatic(void* pd, int data)
{
    RTClientTest* pc = (RTClientTest*)pd;
    if (pc)
    {
        pc->ClientDataCallback(data);
    }
    else
    {
        printf("%s pc is null, line:%d\n", __FUNCTION__, __LINE__);
    }
}

void RTClientTest::ClientDataCallback(int data)
{
    if (data==1)
    {
        printf("ClientDataCallback was called, server sync seqn\n");
        PostDelayed(kPostDelay, this, kRunSyncSeqn);
    } else if (data==2)
    {
        printf("ClientDataCallback was called, server sync data\n");
        PostDelayed(kPostDelay, this, kRunSyncData);
    } else {
        printf("ClientDataCallback was called, data :%d not handle\n\n", data);
    }
}

void RTClientTest::RunSyncSeqn()
{
    printf("RTClientTest::RunSyncSeqn was called\n");
    if (mMsgClient)
        mMsgClient->SyncSeqn();
}

void RTClientTest::RunSyncData()
{
    printf("RTClientTest::RunSyncData was called\n");
    if (mMsgClient)
        mMsgClient->SyncData();
}

void RTClientTest::RunOnce()
{
    //TODO:
    //TestSync();
    //TestSend();
    TestSendOnce();
}

void RTClientTest::TestSend()
{
    int t = 1000000;
    while(t--)
    {
        std::string m("hello world");
        std::string n("xddxdd");
        //client.SendMessage(m);
        mMsgClient->SendMessageTo(m, n);
        rtc::Thread::SleepMs(10);
    }
    t = 10;
    while(t--)
        rtc::Thread::SleepMs(1000);
}

void RTClientTest::TestSendOnce()
{
    int t = 1000000;
    if (mName.compare("xddxdd")==0)
    {
        //while(1)
        //    rtc::Thread::SleepMs(1000);
    } else {
        std::cout << "TestSendOnce name:" << mName << std::endl;
        std::string m("hello world");
        std::string n("xddxdd");
        mMsgClient->SendMessageTo(m, n);
        //t = 15;
        //while(t--)
        //    rtc::Thread::SleepMs(1000);
    }
}

void RTClientTest::TestSync()
{
    int t = 1000000;
    //while(1)
    {
        std::string str("hello world");
        mMsgClient->SendMessage(str);
        mMsgClient->SyncSeqn();
        mMsgClient->SyncData();
        rtc::Thread::SleepMs(1000);
    }
    t = 10;
    while(t--)
        rtc::Thread::SleepMs(1000);
}
