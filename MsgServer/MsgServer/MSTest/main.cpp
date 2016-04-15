//
//  main.cpp
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "rtklog.h"
#include "TUtil.h"


#ifndef _TEST_
#define _TEST_ 0
#endif

int main(int argc, const char * argv[]) {
    printf("Hello, TUtil!!!");

    L_Init(5, NULL);
    TUtil::Initialize(1024);
    TUtil* pTutil = TUtil::Inst();
#if 1
    int res = pTutil->Start();
    int test = 0;
    if (res != 0) {
        LI("Tutil start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    while (test++ < 10) {
        pTutil->DoTick();
        sleep(1);
        //break;
    }
#endif
        sleep(5);
EXIT:
    pTutil->Stop();
    TUtil::DeInitialize();
    L_Deinit();
    printf("main was return here..........\n");
    return 0;
}
