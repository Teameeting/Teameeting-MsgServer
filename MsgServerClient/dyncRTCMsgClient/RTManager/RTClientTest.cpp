//
//  RTClientTest.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTClientTest.hpp"
#include "RTMsgClient.hpp"

const int kStop = 1;
const int kRunOnce = 2;
const int kPostDelay = 3000;

RTClientTest::RTClientTest()
: rtc::MessageHandler()
, rtc::Thread(NULL)
, mIsRun(false)
, mThreadRun(false)
, mTimes(0){

}

RTClientTest::~RTClientTest()
{
    if (mThreadRun)
        this->Stop();
}

void RTClientTest::RunTest(int flag)
{
    mIsRun = true;
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
        PostDelayed(kPostDelay, this, kStop);
    }
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void RTClientTest::RunOnce()
{
    //TODO:
    //TestSync();
    TestSend();
}

void RTClientTest::TestSend()
{
    RTMsgClient client("dandan");
    client.InitSync();
    int t = 1000000;
    //while(t--)
    {
        std::string str("hello world");
        client.SendMessage(str);
        //rtc::Thread::SleepMs(10);
    }
    t = 10;
    while(t--)
        rtc::Thread::SleepMs(1000);
    client.Unin();
}

void RTClientTest::TestSync()
{
    RTMsgClient client("dandan");
    client.InitSync();
    int t = 1000000;
    while(t--)
    {
        client.SyncSeqn();
        client.SyncData();
        rtc::Thread::SleepMs(10);
    }
    t = 10;
    while(t--)
        rtc::Thread::SleepMs(1000);
    client.Unin();
}
