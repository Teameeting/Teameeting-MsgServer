//
//  main.cpp
//  dyncRTMeeting
//
//  Created by hp on 11/20/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "RTMeeting.h"
#include "StrPtrLen.h"


#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    L_Init(5, NULL);
    
    LI("Hello, Meeting!!!");
    RTMeeting::PrintVersion();
    RTMeeting::Initialize(1024);
    RTMeeting* pMeeting = RTMeeting::Inst();
    const char* plocal = "192.168.7.27";
    StrPtrLen pConnAddr((char*)plocal);
    StrPtrLen pQueueAddr((char*)plocal);
    StrPtrLen pHttpAddr((char*)"192.168.7.45");
    unsigned short pConnPort = 9288;
    unsigned short pQueuePort = 29288;
    unsigned short pHttpPort = 8055;
    pMeeting->Start(pConnAddr.GetAsCString(), pConnPort, pQueueAddr.GetAsCString(), pQueuePort, pHttpAddr.GetAsCString(), pHttpPort);
    while (true) {
        pMeeting->DoTick();
        sleep(1);
        //break;
    }
    RTMeeting::DeInitialize();
    L_Deinit();
    return 0;
}
