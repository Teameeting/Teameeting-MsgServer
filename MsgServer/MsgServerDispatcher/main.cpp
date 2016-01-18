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
#include "StrPtrLen.h"


#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    std::cout << "Hello, World!\n";
    L_Init(0, NULL);
    
    LI("Hello, MsgQueue!!!");
    DRTMsgQueue::PrintVersion();
    DRTMsgQueue::Initialize(1024);
    DRTMsgQueue* pMsgQueue = DRTMsgQueue::Inst();
    const char* plocal = "192.168.7.27";
    StrPtrLen pConnIp((char*)plocal);
    StrPtrLen pModuleIp((char*)plocal);
    unsigned short pConnPort = 6620;
    unsigned short pModulePort = 6640;
    pMsgQueue->Start(pConnIp.GetAsCString(), pConnPort, pModuleIp.GetAsCString(), pModulePort);
    while (true) {
        pMsgQueue->DoTick();
        sleep(1);
        //break;
    }
    DRTMsgQueue::DeInitialize();
    L_Deinit();
    return 0;
}
