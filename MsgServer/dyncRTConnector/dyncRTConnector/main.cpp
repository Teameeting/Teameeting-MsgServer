//
//  main.cpp
//  dyncRTConnector
//
//  Created by hp on 11/20/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "RTConnector.h"
#include "StrPtrLen.h"

#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    L_Init(0, NULL);
    
    LI("Hello, Connector!!!");
    RTConnector::PrintVersion();
    RTConnector::Initialize(1024);
    RTConnector* pConnector = RTConnector::Inst();
    const char* plocal = "192.168.7.27";
    StrPtrLen pLanIp((char*)plocal);
    StrPtrLen pModuleIp((char*)plocal);
    StrPtrLen pConnTcpIp((char*)plocal);
    StrPtrLen pHttpIp((char*)"192.168.7.45");
    unsigned short pConnPort = 8892;
    unsigned short pModulePort = 9288;
    unsigned short pConnTcpPort = 9210;
    unsigned short pHttpPort = 6379;
    pConnector->Start(pLanIp.GetAsCString(), pConnPort, pModuleIp.GetAsCString(), pModulePort, pConnTcpIp.GetAsCString(), pConnTcpPort, pHttpIp.GetAsCString(), pHttpPort);
    while (true) {
        pConnector->DoTick();
        sleep(1);
        //break;
    }
    RTConnector::DeInitialize();
    L_Deinit();
    return 0;
}
