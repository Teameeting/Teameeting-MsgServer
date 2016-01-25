//
//  main.cpp
//  MsgServerDispatcher
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "config_parser.h"
#include "rtklog.h"
#include "DRTMsgQueue.h"

#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    LI("Hello, MsgQueue!!!");
    DRTMsgQueue::PrintVersion();

    ConfigSet conf;
    if (argc > 1) {
        conf.LoadFromFile(argv[1]);
    } else {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int debugEnable = conf.GetIntVal("global", "debug", 0);
    std::string strLocalIp("");
    std::string strGlobalIp("");
    int nConnPort = conf.GetIntVal("global", "accept_conn_port", 6620);
    int nDispPort = conf.GetIntVal("global", "listen_disp_port", 6640);
    if (argc > 2) {
         strLocalIp  = argv[2];
         strGlobalIp = argv[2];
    } else {
        strLocalIp  = conf.GetValue("global", "int_ip", "127.0.0.1");
        strGlobalIp = conf.GetValue("global", "ext_ip");
    }
    if (strLocalIp.length()==0 || strGlobalIp.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int log_level = conf.GetIntVal("log", "level", 5);
    std::string strLogPath = conf.GetValue("log", "path");
    if (log_level < 0 || log_level > 5) {
        std::cout << "Error: Log level=" << log_level << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    L_Init(0, NULL);
    DRTMsgQueue::Initialize(1024);
    DRTMsgQueue* pMsgQueue = DRTMsgQueue::Inst();
    pMsgQueue->Start(strLocalIp.c_str(), nConnPort, strLocalIp.c_str(), nDispPort);
    while (true) {
        pMsgQueue->DoTick();
        sleep(1);
        //break;
    }
    DRTMsgQueue::DeInitialize();
    L_Deinit();
    return 0;
}
