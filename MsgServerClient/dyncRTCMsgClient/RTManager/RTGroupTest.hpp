//
//  RTGroupTest.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTGroupTest_hpp
#define RTGroupTest_hpp

#include <stdio.h>
#include "webrtc/base/thread.h"
#include "webrtc/base/messagequeue.h"
#include "RTMsgClient.hpp"

class RTGroupTest
: public rtc::MessageHandler
, public rtc::Thread{
public:
    RTGroupTest();
    ~RTGroupTest();

    void RunTest(int flag, const std::string& name);
    virtual void OnMessage(rtc::Message* msg);


    void RunSyncSeqn();
    void RunSyncData();

    void ClientDataCallback(int data);
    static void ClientDataCallbackStatic(void* pd, int data);

public:
    void RunOnce();
private:
    bool    mIsRun;
    bool    mThreadRun;
    int     mTimes;
    std::string mName;
    RTMsgClient*    mMsgClient;
};

#endif /* RTGroupTest_hpp */
