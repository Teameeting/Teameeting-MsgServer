//
//  main.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include <google/protobuf/message.h>
#include "webrtc/base/logging.h"
#include "RTConnClientManager.hpp"
#include "RTClientTest.hpp"
#include "RTGroupTest.hpp"

#define KRUN 1
#define KRUNONCE 2
#define KRUNTHREAD 3
#define KRUNAPPLYROOM 4
#define KRUNLOGINOUT 5
#define KRUNSENDMSG 6

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";

    if (argc<3) {
        std::cout << "please add param username and group or single!!!" << std::endl;
         return 0;
    }
    rtc::LogMessage::LogToDebug(rtc::INFO);
    //RTConnClientManager manager;
    //manager.RunTest(KRUN);
    if (strcmp(argv[2], "group")==0)
    {
        RTGroupTest gtest;
        gtest.RunTest(KRUNONCE, argv[1]);
    } else {
        RTClientTest test;
        test.RunTest(KRUNONCE, argv[1]);
    }
    google::protobuf::ShutdownProtobufLibrary();
    LOG(INFO) << "bye bye client...";
    return 0;
}
