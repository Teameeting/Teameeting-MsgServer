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
    const char* plocal = "127.0.0.1";
    StrPtrLen pConnIp((char*)plocal);
    StrPtrLen pModuleIp((char*)plocal);
    char* ppConnIp = pConnIp.GetAsCString();
    char* ppModuleIp = pModuleIp.GetAsCString();
    unsigned short pConnPort = 6620;
    unsigned short pModulePort = 6640;
    pMsgQueue->Start(ppConnIp, pConnPort, ppModuleIp, pModulePort);
    delete ppConnIp;
    ppConnIp = NULL;
    delete ppModuleIp;
    ppModuleIp = NULL;
    while (true) {
        pMsgQueue->DoTick();
        sleep(1);
        //break;
    }
    DRTMsgQueue::DeInitialize();
    L_Deinit();
    return 0;
}
