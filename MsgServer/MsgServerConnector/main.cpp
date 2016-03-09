//
//  main.cpp
//  MsgServerConnector
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "CRTConnector.h"
#include "RTZKClient.hpp"

#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    LI("Hello, Connector!!!");
    CRTConnector::PrintVersion();
    
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
    CRTConnector::Initialize(1024);
    CRTConnector* pConnector = CRTConnector::Inst();
    pConnector->Start(c.GetServerConfig().IP.c_str(),
                      c.GetServerConfig().portConfig.connector.ListenWebcon,
                      c.GetServerConfig().IP.c_str(),
                      c.GetServerConfig().portConfig.connector.ListenModule,
                      c.GetServerConfig().IP.c_str(),
                      c.GetServerConfig().portConfig.connector.ListenClicon
                      );
    while (true) {
        pConnector->DoTick();
        sleep(1);
        //break;
    }
    CRTConnector::DeInitialize();
    L_Deinit();
    return 0;
}
