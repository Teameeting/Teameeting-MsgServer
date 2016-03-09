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

#ifndef _DEBUG
#define _DEBUG 0
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
    RTZKClient c(argv[1]);
    if (c.InitZKClient()!=0) {
        std::cout << "Please check the config file ..." << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
    
    L_Init(0, NULL);
    DRTMsgQueue::Initialize(1024);
    DRTMsgQueue* pMsgQueue = DRTMsgQueue::Inst();
    pMsgQueue->Start(c.GetServerConfig().IP.c_str(),
                      c.GetServerConfig().portConfig.dispatcher.AcceptConn,
                      c.GetServerConfig().IP.c_str(),
                      c.GetServerConfig().portConfig.dispatcher.ListenDisp,
                      c.GetServerConfig().HttpIp.c_str(),
                      c.GetServerConfig().portConfig.dispatcher.ListenHttp
                      );
    
    while (true) {
        pMsgQueue->DoTick();
        sleep(1);
        //break;
    }
    DRTMsgQueue::DeInitialize();
    L_Deinit();
    return 0;
}
