//
//  main.cpp
//  MsgServerMeeting
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "MRTMeeting.h"

#include "RTZKClient.hpp"

#ifndef _TEST_
#define _TEST_ 0
#endif

int main(int argc, const char * argv[]) {
    LI("Hello, Meeting!!!");
    MRTMeeting::PrintVersion();

    if (argc <= 1) {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
#if _TEST_
    if (RTZKClient::Instance()->InitOnly(argv[1])!=0) {
#else
    if (RTZKClient::Instance()->InitZKClient(argv[1])!=0) {
#endif
        std::cout << "Please check the config file ..." << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
    
    L_Init(5, NULL);
    MRTMeeting::Initialize(1024);
    MRTMeeting* pMeeting = MRTMeeting::Inst();
    pMeeting->Start(RTZKClient::Instance()->GetServerConfig().IP.c_str(),
                    RTZKClient::Instance()->GetServerConfig().portConfig.meeting.AcceptConn,
                    RTZKClient::Instance()->GetServerConfig().IP.c_str(),
                    RTZKClient::Instance()->GetServerConfig().portConfig.meeting.AcceptDisp,
                    RTZKClient::Instance()->GetServerConfig().HttpIp.c_str(),
                    RTZKClient::Instance()->GetServerConfig().portConfig.meeting.AcceptHttp
                    );

    while (true) {
        pMeeting->DoTick();
        sleep(1);
        //break;
    }
    MRTMeeting::DeInitialize();
    L_Deinit();
    return 0;
}
