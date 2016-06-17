//
//  RTClientTest.hpp
//  dyncRTCMsgClient
//
//  Created by hp on 2/23/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef RTClientTest_hpp
#define RTClientTest_hpp

#include <stdio.h>
#include "webrtc/base/thread.h"
#include "webrtc/base/messagequeue.h"

class RTClientTest
: public rtc::MessageHandler
, public rtc::Thread{
public:
    RTClientTest();
    ~RTClientTest();

    void RunTest(int flag, const std::string& name);
    virtual void OnMessage(rtc::Message* msg);

    void TestSync();
    void TestSend();
    void TestSendOnce();
public:
    void RunOnce();
private:
    bool    mIsRun;
    bool    mThreadRun;
    int     mTimes;
    std::string mName;
};

#endif /* RTClientTest_hpp */
