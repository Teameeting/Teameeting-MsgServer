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
    const char* plocal = "127.0.0.1";
    StrPtrLen pLanIp((char*)plocal);
    StrPtrLen pModuleIp((char*)plocal);
    StrPtrLen pConnTcpIp((char*)plocal);
    StrPtrLen pHttpIp((char*)"192.168.7.45");
    char* ppLanIp = pLanIp.GetAsCString();
    char* ppModuleIp = pModuleIp.GetAsCString();
    char* ppConnTcpIp = pConnTcpIp.GetAsCString();
    char* ppHttpIp = pHttpIp.GetAsCString();
    if (!ppLanIp | !ppModuleIp | !ppConnTcpIp | !ppHttpIp)
        return -1;
    unsigned short pConnPort = 6610;
    unsigned short pModulePort = 6620;
    unsigned short pConnTcpPort = 6630;
    unsigned short pRedisPort = 6379;
    pConnector->Start(ppLanIp, pConnPort, ppModuleIp, pModulePort, ppConnTcpIp, pConnTcpPort, ppHttpIp, pRedisPort);
    delete ppLanIp;
    ppLanIp = NULL;
    delete ppModuleIp;
    ppModuleIp = NULL;
    delete ppConnTcpIp;
    ppConnTcpIp = NULL;
    delete ppHttpIp;
    ppHttpIp = NULL;
    int n = 0;
    while (true) {
        pConnector->DoTick();
        sleep(1);
        //if (n++>10)
        //break;
    }
    CRTConnector::DeInitialize();
    L_Deinit();
    return 0;
}
