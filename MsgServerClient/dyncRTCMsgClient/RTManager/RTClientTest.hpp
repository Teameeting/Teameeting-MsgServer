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

    void RunTest(int flag);
    virtual void OnMessage(rtc::Message* msg);
public:
    void RunOnce();
private:
    bool    mIsRun;
    bool    mThreadRun;
    int     mTimes;
};

#endif /* RTClientTest_hpp */
