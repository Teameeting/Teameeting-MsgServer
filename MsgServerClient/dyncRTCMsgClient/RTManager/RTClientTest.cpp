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

void RTClientTest::RunTest(int flag, const std::string& name)
{
    mIsRun = true;
    mName = name;
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
    //TestSend();
    TestSendOnce();
}

void RTClientTest::TestSend()
{
    RTMsgClient client("dandan");
    client.InitSync();
    int t = 1000000;
    while(t--)
    {
        std::string m("hello world");
        std::string n("xddxdd");
        //client.SendMessage(m);
        client.SendMessageTo(m, n);
        rtc::Thread::SleepMs(10);
    }
    t = 10;
    while(t--)
        rtc::Thread::SleepMs(1000);
    client.Unin();
}

void RTClientTest::TestSendOnce()
{
    RTMsgClient client(mName);
    client.InitSync();
    int t = 1000000;
    {
    }
    if (mName.compare("xddxdd")==0)
    {
        while(1)
            rtc::Thread::SleepMs(1000);
    } else {
        std::cout << "TestSendOnce name:" << mName << std::endl;
        std::string m("hello world");
        std::string n("xddxdd");
        client.SendMessageTo(m, n);
        t = 15;
        while(t--)
            rtc::Thread::SleepMs(1000);
    }
    client.Unin();
}

void RTClientTest::TestSync()
{
    RTMsgClient client("dandan");
    client.InitSync();
    int t = 1000000;
    while(1)
    {
        std::string str("hello world");
        client.SendMessage(str);
        client.SyncSeqn();
        client.SyncData();
        rtc::Thread::SleepMs(1000);
    }
    t = 10;
    while(t--)
        rtc::Thread::SleepMs(1000);
    client.Unin();
}
