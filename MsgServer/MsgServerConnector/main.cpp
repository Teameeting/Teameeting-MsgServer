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
#include "StrPtrLen.h"

#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    L_Init(0, NULL);
    
    LI("Hello, Connector!!!");
    CRTConnector::PrintVersion();
    CRTConnector::Initialize(1024);
    CRTConnector* pConnector = CRTConnector::Inst();
    const char* plocal = "192.168.7.27";
    StrPtrLen pLanIp((char*)plocal);
    StrPtrLen pModuleIp((char*)plocal);
    StrPtrLen pConnTcpIp((char*)plocal);
    StrPtrLen pHttpIp((char*)"192.168.7.45");
    unsigned short pConnPort = 6610;
    unsigned short pModulePort = 6620;
    unsigned short pConnTcpPort = 6630;
    unsigned short pRedisPort = 6379;
    pConnector->Start(pLanIp.GetAsCString(), pConnPort, pModuleIp.GetAsCString(), pModulePort, pConnTcpIp.GetAsCString(), pConnTcpPort, pHttpIp.GetAsCString(), pRedisPort);
    while (true) {
        pConnector->DoTick();
        sleep(1);
        //break;
    }
    CRTConnector::DeInitialize();
    L_Deinit();
    return 0;
}
