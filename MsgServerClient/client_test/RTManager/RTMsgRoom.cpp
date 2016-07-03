//
//  RTMsgRoom.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/25/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "RTMsgRoom.hpp"
#include <iostream>
#include <sstream>
#include <string>



int gTestClientNum = 2;
int gTestMessageNum = 5;
int gTestLoginNum = 10;

RTMsgRoom::RTMsgRoom(int num, std::string& id)
: mRoomNum(num)
, mRoomId(id)
, mIsRun(false){

}

RTMsgRoom::~RTMsgRoom()
{

}

void RTMsgRoom::Before()
{
    for (int i=0; i<gTestClientNum; ++i) {
        char cn[16] = {0};
        sprintf(cn, "room%d-user%d", mRoomNum, i);
        RTMsgClient *c = new RTMsgClient(cn);
        c->Register();
        c->Init(pms::EModuleType::TMEETING);
        c->Connecting();
        c->SetRoomId(mRoomId);
        mClientList.push_back(c);
        printf("push number %d client userid:%s\n", i, c->GetUserId().c_str());
        rtc::Thread::SleepMs(1000);
    }
}

void RTMsgRoom::After()
{
    for (auto lit=mClientList.begin(); lit!=mClientList.end(); ++lit) {
        RTMsgClient *c = *lit;
        rtc::Thread::SleepMs(500);
        c->Unin();
        printf("client userid:%s leave room, GetRecvNums:%d\n", c->GetUserId().c_str(), c->GetRecvNums());
        rtc::Thread::SleepMs(1000);
    }
    for (auto& x : mClientList) {
        x->ShowRecvMsg();
        rtc::Thread::SleepMs(500);
    }
    for (auto& x : mClientList) {
        delete x;
        x = NULL;
        printf("After delete client\n");
    }
    mClientList.clear();
}

void RTMsgRoom::RunRun()
{
    Before();
    long long num = 0;
    mIsRun = true;
    while (mIsRun) {
        //++num;
        TestMsg(++num);
        if (num==gTestMessageNum) {
            mIsRun = false;
        }
        rtc::Thread::SleepMs(100);
    }
    rtc::Thread::SleepMs(1000);
    After();
}

void RTMsgRoom::RunOnce()
{
    RTMsgClient client("test1");
    client.Register();
    client.ApplyRoom();
    std::string msg = "this is a test msg";
    client.Init(pms::EModuleType::TMEETING);
    client.Connecting();
    while (1) {
        client.SendMsg(msg);
        rtc::Thread::SleepMs(3000);
        break;
    }
    rtc::Thread::SleepMs(1000);
    client.Unin();
    rtc::Thread::SleepMs(1000);
}

void RTMsgRoom::RunThread()
{
    Before();
    long long num = 0;
    mIsRun = true;
    while (mIsRun) {
        TestMsg(++num);
        if (num==gTestMessageNum) {
            mIsRun = false;
        }
        rtc::Thread::SleepMs(500);
    }
    rtc::Thread::SleepMs(3000);
    After();
}

void RTMsgRoom::RunApplyRoom()
{
    int room = 100;
    int mem = 20;
    for (int i=0; i<mem; ++i) {
        char cn[16] = {0};
        sprintf(cn, "room%d-user%d", i*2, i);
        RTMsgClient client(cn);
        if (client.Register()==0) {
            for (int j=0; j<room; ++j) {
                client.ApplyRoom();
                rtc::Thread::SleepMs(1000);
                printf("RTMsgRoom::RunApplyRoom i:%d, j:%d\n", i,j);
            }
        }
    }
}
void RTMsgRoom::RunLoginout()
{
    TestInit();
    TestLogout();
    TestLogin();
    TestLogout();
    TestLogin();
    TestUnin();
}

void RTMsgRoom::RunSendMsg()
{

}

void RTMsgRoom::TestMsg(long long flag)
{
    std::ostringstream oss("");
    int i = (flag / 10) % 2, j = flag % 10, m = 0;
    //printf("TestMsg i:%d, j:%d, flag:%lld\n", i, j, flag);
    for (auto lit=mClientList.begin(); lit!=mClientList.end(); ++lit) {
        m++;
        if (i==0 && m==j) {// flag is jishu
            //if (j==2 || j==4 || j==6) {
            if (j==2 || j==6) {
            }
        } else if (i==1 && m==j) {// flag is oushu
            //if (j==2 || j==4 || j==6) {
            if (j==2 || j==6) {
            }
        }
        oss.str("");
        oss << (*lit)->GetUserId() + " send msg num: " << flag;
        (*lit)->SendMsg(oss.str());
    }
}

void RTMsgRoom::TestLogin()
{
    int i = 0;
    RTMsgClient *p = NULL;
    for (auto lit=mClientList.begin(); lit!=mClientList.end(); ++lit) {
        if ((++i % 2)  == 0) {
            continue;
        }
        p = *lit;
        p->Init(pms::EModuleType::TMEETING);
        p->Connecting();
        rtc::Thread::SleepMs(100);
    }
}

void RTMsgRoom::TestLogout()
{
    int i = 0;
    RTMsgClient *p = NULL;
    for (auto lit=mClientList.begin(); lit!=mClientList.end(); ++lit) {
        if ((++i % 2)  == 0) {
            continue;
        }
        p = *lit;
        p->Unin();
        rtc::Thread::SleepMs(100);
    }
}

void RTMsgRoom::TestInit()
{
    for (int i=0; i<gTestLoginNum; ++i) {
        char cn[16] = {0};
        sprintf(cn, "room%d-user%d", mRoomNum, i);
        RTMsgClient *c = new RTMsgClient(cn);
        c->Register();
        c->Init(pms::EModuleType::TMEETING);
        c->Connecting();
        mClientList.push_back(c);
        printf("RTMsgRoom::TestInit number %d client userid:%s\n", i, c->GetUserId().c_str());
        rtc::Thread::SleepMs(100);
    }
}

void RTMsgRoom::TestUnin()
{
    for (auto lit=mClientList.begin(); lit!=mClientList.end(); ++lit) {
        RTMsgClient *c = *lit;
        rtc::Thread::SleepMs(100);
        c->Unin();
        printf("RTMsgRoom::TestUnit userid:%s leave room, GetConnNums:%d, GetDisconnNums:%d, GetConnFailNums:%d\n"
               , c->GetUserId().c_str()
               , c->GetConnNums()
               , c->GetDisconnNums()
               , c->GetConnFailNums());
        rtc::Thread::SleepMs(100);
    }
    for (auto& x : mClientList) {
        delete x;
    }
    mClientList.clear();
}

/////////////////////////////////////////////////////
//////////////////private////////////////////////////
/////////////////////////////////////////////////////

