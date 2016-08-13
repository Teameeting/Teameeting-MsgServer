//
//  main.cpp
//  MsgServerSequence
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "SRTSequence.h"
#include "RTZKClient.hpp"
#include <google/protobuf/message.h>

#ifndef _TEST_
#define _TEST_ 1
#endif

int main(int argc, const char * argv[]) {
    printf("Hello, Sequence!!!\n");
    SRTSequence::PrintVersion();

    if (argc <= 1) {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
#if 0
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
#endif

    MsConfigParser conf;
    conf.LoadFromFile(argv[2]);

    L_Init(0, NULL);
    SRTSequence::Initialize(1024);
    SRTSequence* pSequence = SRTSequence::Inst();
    int res = pSequence->Start(conf);
    if (res != 0) {
        //////LI("SRTSequence start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //int test = 0;
    //while (test++ < 25) {
    while (1) {
        pSequence->DoTick();
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pSequence->Stop();
    SRTSequence::DeInitialize();
    L_Deinit();
    //RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
