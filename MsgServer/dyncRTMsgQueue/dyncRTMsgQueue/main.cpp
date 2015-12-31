//
//  main.cpp
//  dyncRTMsgQueue
//
//  Created by hp on 11/20/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <iostream>
#include "rtklog.h"
#include "RTMsgQueue.h"
#include "StrPtrLen.h"


#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    L_Init(0, NULL);
    
    LI("Hello, MsgQueue!!!");
    RTMsgQueue::PrintVersion();
    RTMsgQueue::Initialize(1024);
    RTMsgQueue* pMsgQueue = RTMsgQueue::Inst();
    const char* plocal = "192.168.7.27";
    StrPtrLen pConnIp((char*)plocal);
    StrPtrLen pModuleIp((char*)plocal);
    unsigned short pConnPort = 9288;
    unsigned short pModulePort = 29288;
    pMsgQueue->Start(pConnIp.GetAsCString(), pConnPort, pModuleIp.GetAsCString(), pModulePort);
    while (true) {
        pMsgQueue->DoTick();
        sleep(1);
        //break;
    }
    RTMsgQueue::DeInitialize();
    L_Deinit();
    return 0;
}
