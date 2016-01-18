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
    const char* plocal = "192.168.7.27";
    StrPtrLen pConnAddr((char*)plocal);
    StrPtrLen pQueueAddr((char*)plocal);
    StrPtrLen pHttpAddr((char*)"192.168.7.45");
    unsigned short pConnPort = 6620;
    unsigned short pQueuePort = 6640;
    unsigned short pHttpPort = 8055;
    pMeeting->Start(pConnAddr.GetAsCString(), pConnPort, pQueueAddr.GetAsCString(), pQueuePort, pHttpAddr.GetAsCString(), pHttpPort);
    while (true) {
        pMeeting->DoTick();
        sleep(1);
        //break;
    }
    MRTMeeting::DeInitialize();
    L_Deinit();
    return 0;
}
