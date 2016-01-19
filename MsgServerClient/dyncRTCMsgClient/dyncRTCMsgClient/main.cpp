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
    
    virtual void OnSndMsg(const std::string& msg) {
        //LOG(INFO) << __FUNCTION__ << " msg:" << msg;
        MEETMSG mmsg;
        std::string err;
        mmsg.GetMsg(msg, err);
        if (err.length()>0) {
            LOG(INFO) << "OnSndMsg err:" << err;
            return;
        }
        if (mmsg._messagetype==MESSAGETYPE::request) {
            switch(mmsg._tags) {
                case SENDTAGS::sendtags_talk:
                {
                    LOG(INFO) << "sendtags_talk " << mmsg._cont;
                }
                    break;
                case SENDTAGS::sendtags_enter:
                {
                    LOG(INFO) << "sendtags_enter " << mmsg._cont;
                }
                    break;
                case SENDTAGS::sendtags_leave:
                {
                    LOG(INFO) << "sendtags_leave " << mmsg._cont;
                }
                    break;
                case SENDTAGS::sendtags_subscribe:
                {
                    LOG(INFO) << "sendtags_subscribe " << mmsg._cont;
                }
                    break;
                case SENDTAGS::sendtags_unsubscribe:
                {
                    LOG(INFO) << "sendtags_unsubscribe " << mmsg._cont;
                }
                    break;
                default:
                {
                    
                }
                    break;
            }
        }
    }

    virtual void OnGetMsg(const std::string& msg) {
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
    
    virtual void OnMsgServerState(MSTcpState state) {
        LOG(INFO) << __FUNCTION__ << " was called, state:" << state;
    }

    virtual ~MsgServerCallback(){}
};

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    rtc::LogMessage::LogToDebug(rtc::INFO);
    XMsgClient client;
    MsgServerCallback callback;
    const std::string server("192.168.7.39");
    int port = 9210;
    //bool autoConnect = true;
    LOG(INFO) << "begin connect to server...";
    //std::string userid("9a4f3730-f643-422a-a3a1-eae557060a90");
    //std::string pass("a405f2ad61030c3e354a144137213f819d32516896d7ed883d1dfb05dcd993bd8578d422bbf1e84f5cce15316374a217");
    std::string userid("8ca64d158a505876");
    std::string pass("7d6a1cafeb40616ddf0c7d490771330f98a8741ae92fc450a28b2a27ebf9156a");
    std::string roomid("400000000436");
    std::string msg("hello world, are you ok?");
    client.Init(callback, userid, pass, server, port);
    while(client.MSStatus()!=MSTcpState::MSCONNECTED)sleep(1);
    LOG(INFO) << "connectted to server...";
    client.OptRoom(MEETCMD::enter, roomid, "");
    while (1) {
        //LOG(INFO) << "pClient->Status:" << client.Status();
        client.SndMsg(roomid, msg);
        client.NotifyMsg(roomid, "tagstagstagstags");
        rtc::Thread::SleepMs(3000);
        //break;
    }
    client.OptRoom(MEETCMD::leave, roomid, "");
    rtc::Thread::SleepMs(3000);
    client.Unin();
    LOG(INFO) << "bye bye client...";
    return 0;
}
