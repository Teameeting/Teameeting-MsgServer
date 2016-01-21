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
#include "StrPtrLen.h"


#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    L_Init(5, NULL);

    LI("Hello, Meeting!!!");
    MRTMeeting::PrintVersion();
    MRTMeeting::Initialize(1024);
    MRTMeeting* pMeeting = MRTMeeting::Inst();
    const char* plocal = "127.0.0.1";
    StrPtrLen pConnAddr((char*)plocal);
    StrPtrLen pQueueAddr((char*)plocal);
    StrPtrLen pHttpAddr((char*)"192.168.7.45");
    char* ppConnAddr = pConnAddr.GetAsCString();
    char* ppQueueAddr = pQueueAddr.GetAsCString();
    char* ppHttpAddr = pHttpAddr.GetAsCString();
    unsigned short pConnPort = 6620;
    unsigned short pQueuePort = 6640;
    unsigned short pHttpPort = 8055;
    pMeeting->Start(ppConnAddr, pConnPort, ppQueueAddr, pQueuePort, ppHttpAddr, pHttpPort);
    delete ppConnAddr;
    ppConnAddr = NULL;
    delete ppQueueAddr;
    ppQueueAddr = NULL;
    delete ppHttpAddr;
    ppHttpAddr = NULL;
    while (true) {
        pMeeting->DoTick();
        sleep(1);
        //break;
    }
    MRTMeeting::DeInitialize();
    L_Deinit();
    return 0;
}
