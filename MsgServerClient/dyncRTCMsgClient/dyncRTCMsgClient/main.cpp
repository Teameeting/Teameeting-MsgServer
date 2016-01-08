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
    std::string userid("9a4f3730-f643-422a-a3a1-eae557060a90");
    std::string pass("a405f2ad61030c3e354a144137213f819d32516896d7ed883d1dfb05dcd993bd8578d422bbf1e84f5cce15316374a217");
    std::string roomid("400000000436");
    std::string msg("hello world, are you ok?");
    client.Init(callback, userid, pass, server, port);
    LOG(INFO) << "connectted to server...";
    while(client.Status()!=TcpState::CONNECTED)sleep(1);
    //client.OptRoom(MEETCMD::create, userid, pass, roomid, "");
    client.OptRoom(MEETCMD::enter, roomid, "");
    while (1) {
        //LOG(INFO) << "pClient->Status:" << client.Status();
        client.SndMsg(roomid, msg);
        rtc::Thread::SleepMs(3000);
        break;
    }
    client.OptRoom(MEETCMD::leave, roomid, "");
    //client.OptRoom(MEETCMD::destroy, userid, pass, roomid, "");
    rtc::Thread::SleepMs(3000);
    client.Unin();
    LOG(INFO) << "bye bye client...";
    return 0;
}
