//
//  RTClientTest.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTClientTest.hpp"

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
    if (name.compare("xddxdd")==0)
    {
        mMsgClient->TestSetCurSeqn(290945);
    } else {
        mMsgClient->TestSetCurSeqn(337055);
    }
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
        PostDelayed(kPostDelay, this, kRunSyncSeqn);
    } else {
        printf("ClientDataCallback was called, data :%d not handle\n\n", data);
    }
}

void RTClientTest::RunSyncSeqn()
{
    if (mMsgClient)
        mMsgClient->SyncSeqn();
}

void RTClientTest::RunSyncData()
{
    if (mMsgClient)
        mMsgClient->SyncData();
}

void RTClientTest::RunOnce()
{
    //TODO:
    //TestSync();
    //TestSend();
    ////if (mName.compare("xddxdd")==0)
    ////{
    //    //TestSyncGroup();
    ////} else {
    //    //TestSendGroup();
    ////}
    //TestSendOnce();
    //TestSendLoop();
    TestSendGroupLoop();
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

void RTClientTest::TestSendGroup()
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
            mMsgClient->SendMessage(m);
            rtc::Thread::SleepMs(500);
        }
    }
}

void RTClientTest::TestSyncGroup()
{
    if (mName.compare("xddxdd")==0)
    {
        mMsgClient->GrpSyncGroupData("xddxdd", "4008696859", 10);
        //while(1)
        //    rtc::Thread::SleepMs(1000);
    } else {
        {
            std::cout << "TestSyncGroup name:" << mName << std::endl;

            rtc::Thread::SleepMs(500);
        }
    }
}

void RTClientTest::TestSendOnce()
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

void RTClientTest::TestSendLoop()
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

void RTClientTest::TestSendGroupLoop()
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
            std::cout << "TestSendGroupLoop name:" << mName << std::endl;
            sprintf(ii, "hello world%lld", i++);
            std::string m(ii);
            std::string n("xddxdd");
            mMsgClient->SendMessage(m);
            rtc::Thread::SleepMs(200);
        }
    }
}

void RTClientTest::TestSync()
{
    //while(1)
    {
        std::string str("hello world");
        mMsgClient->SendMessage(str);
        mMsgClient->SyncSeqn();
        mMsgClient->SyncData();
        rtc::Thread::SleepMs(1000);
    }
    int t = 10;
    while(t--)
        rtc::Thread::SleepMs(1000);
}
