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
    if (RTZKClient::Instance()->InitOnly(argv[1])!=0) {
#else
    if (RTZKClient::Instance()->InitZKClient(argv[1])!=0) {
#endif
        std::cout << "Please check the config file ..." << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    L_Init(0, NULL);
    DRTMsgQueue::Initialize(1024);
    DRTMsgQueue* pMsgQueue = DRTMsgQueue::Inst();
    int res = pMsgQueue->Start(RTZKClient::Instance()->GetServerConfig().IP.c_str(),
                     RTZKClient::Instance()->RTZKClient::Instance()->GetServerConfig().portConfig.dispatcher.AcceptConn,
                     RTZKClient::Instance()->GetServerConfig().IP.c_str(),
                     RTZKClient::Instance()->GetServerConfig().portConfig.dispatcher.ListenDisp,
                     RTZKClient::Instance()->GetServerConfig().HttpIp.c_str(),
                     RTZKClient::Instance()->GetServerConfig().portConfig.dispatcher.ListenHttp
                     );
    if (res != 0) {
        LI("DRTMsgQueue start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    while (true) {
        pMsgQueue->DoTick();
        sleep(1);
        //break;
    }
EXIT:
    DRTMsgQueue::DeInitialize();
    L_Deinit();
    return 0;
}
