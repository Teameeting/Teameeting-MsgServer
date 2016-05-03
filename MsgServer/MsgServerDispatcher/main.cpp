//
//  main.cpp
//  MsgServerDispatcher
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "DRTMsgQueue.h"
#include "RTZKClient.hpp"
#include <google/protobuf/message.h>

#ifndef _TEST_
#define _TEST_ 0
#endif

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    LI("Hello, MsgQueue!!!");
    DRTMsgQueue::PrintVersion();

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

    DRTMsgQueue::Initialize(1024);
    DRTMsgQueue* pMsgQueue = DRTMsgQueue::Inst();
    int res = pMsgQueue->Start(RTZKClient::Instance().GetServerConfig().IP.c_str(),
                     RTZKClient::Instance().RTZKClient::Instance().GetServerConfig().portConfig.dispatcher.AcceptConn,
                     RTZKClient::Instance().GetServerConfig().IP.c_str(),
                     RTZKClient::Instance().GetServerConfig().portConfig.dispatcher.ListenDisp,
                     RTZKClient::Instance().GetServerConfig().HttpIp.c_str(),
                     RTZKClient::Instance().GetServerConfig().portConfig.dispatcher.ListenHttp
                     );
    int test = 0;
    if (res != 0) {
        LI("DRTMsgQueue start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 100) {
    while (1) {
        pMsgQueue->DoTick();
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pMsgQueue->Stop();
    DRTMsgQueue::DeInitialize();
    L_Deinit();
    RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
