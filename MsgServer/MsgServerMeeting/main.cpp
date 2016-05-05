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
#include <google/protobuf/message.h>

#ifndef _TEST_
#define _TEST_ 1
#endif

int main(int argc, const char * argv[]) {
    printf("Hello, Meeting!!!");
    MRTMeeting::PrintVersion();

    if (argc <= 1) {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
#if _TEST_
    if (RTZKClient::Instance().InitOnly(argv[1])!=0) {
#else
    if (RTZKClient::Instance().InitZKClient(argv[1])!=0) {
#endif
        std::cout << "Please check the config file ..." << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int level = RTZKClient::Instance().GetServerConfig().Level;
    std::string logpath = RTZKClient::Instance().GetServerConfig().LogPath;
    if (logpath.empty())
        L_Init(level, NULL);
    else
        L_Init(level, logpath.c_str());

    MRTMeeting::Initialize(1024);
    MRTMeeting* pMeeting = MRTMeeting::Inst();
#if 1
    int res = pMeeting->Start(RTZKClient::Instance().GetServerConfig().IP.c_str(),
                    RTZKClient::Instance().GetServerConfig().portConfig.meeting.AcceptConn,
                    RTZKClient::Instance().GetServerConfig().IP.c_str(),
                    RTZKClient::Instance().GetServerConfig().portConfig.meeting.AcceptDisp,
                    RTZKClient::Instance().GetServerConfig().HttpIp.c_str(),
                    RTZKClient::Instance().GetServerConfig().portConfig.meeting.AcceptHttp
                    );
    int test = 0;
    if (res != 0) {
        LI("MRTMeeting start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 80) {
    while (1) {
        pMeeting->DoTick();
        sleep(1);
        //break;
    }
#endif
        sleep(1);
EXIT:
    pMeeting->Stop();
    MRTMeeting::DeInitialize();
    L_Deinit();
    RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
