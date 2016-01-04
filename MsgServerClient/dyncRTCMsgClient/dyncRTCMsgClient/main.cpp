//
//  main.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "webrtc/base/logging.h"
#include "XMsgClient.h"
#include "XMsgCallback.h"

class MsgServerCallback : public XMsgCallback{
public:
    
    virtual void OnReqLogin(int code, const std::string& status, const std::string& userid) {
        LOG(INFO) << __FUNCTION__ << " code:" << code << " status:" << status << " userid:" << userid;
    }

    virtual void OnRespLogin(int code, const std::string& status, const std::string& userid) {
        LOG(INFO) << __FUNCTION__ << " code:" << code << " status:" << status << " userid:" << userid;
    }

    virtual void OnReqSndMsg(const std::string& msg) {
        LOG(INFO) << __FUNCTION__ << " msg:" << msg;
    }

    virtual void OnRespSndMsg(const std::string& msg) {
        LOG(INFO) << __FUNCTION__ << " msg:" << msg;
    }

    virtual void OnReqGetMsg(const std::string& msg) {
        LOG(INFO) << __FUNCTION__ << " msg:" << msg;
    }

    virtual void OnRespGetMsg(const std::string& msg) {
        LOG(INFO) << __FUNCTION__ << " msg:" << msg;
    }

    virtual void OnReqLogout(int code, const std::string& status, const std::string& userid) {
        LOG(INFO) << __FUNCTION__ << " code:" << code << " status:" << status << " userid:" << userid;
    }

    virtual void OnRespLogout(int code, const std::string& status, const std::string& userid) {
        LOG(INFO) << __FUNCTION__ << " code:" << code << " status:" << status << " userid:" << userid;
    }
    
    virtual void OnMsgServerConnected() {
        LOG(INFO) << __FUNCTION__ << " was called";
    }

    virtual void OnMsgServerDisconnect() {
        LOG(INFO) << __FUNCTION__ << " was called";
    }

    virtual void OnMsgServerConnectionFailure() {
        LOG(INFO) << __FUNCTION__ << " was called";
    }



    virtual ~MsgServerCallback(){}
};

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    rtc::LogMessage::LogToDebug(rtc::INFO);
    XMsgClient client;
    MsgServerCallback callback;
    const std::string server("192.168.7.27");
    int port = 9210;
    //bool autoConnect = true;
    LOG(INFO) << "begin connect to server...";
    std::string userid("test001");
    std::string pass("123456");
    std::string roomid("roomid");
    std::string msg("hello world, are you ok?");
    client.Init(callback, server, port);
    LOG(INFO) << "connectted to server...";
    while(client.Status()!=TcpState::CONNECTED)sleep(1);
    client.Login(userid, pass);
    client.OptRoom(MEETCMD::create, userid, pass, roomid);
    client.OptRoom(MEETCMD::enter, userid, pass, roomid);
    while (1) {
        //LOG(INFO) << "pClient->Status:" << client.Status();
        client.SndMsg(userid, pass, roomid, msg);
        rtc::Thread::SleepMs(3000);
        //break;
    }
    client.OptRoom(MEETCMD::leave, userid, pass, roomid);
    client.OptRoom(MEETCMD::destroy, userid, pass, roomid);
    client.Logout(userid, pass);
    rtc::Thread::SleepMs(3000);
    client.Unin();
    LOG(INFO) << "bye bye client...";
    return 0;
}
