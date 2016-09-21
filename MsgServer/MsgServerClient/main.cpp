//
//  main.cpp
//  MsgServerClient
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "rtklog.h"
#include "MsgServerClient.h"
#include <google/protobuf/message.h>
#include <time.h>

#ifndef _TEST_
#define _TEST_ 1
#endif

int main(int argc, const char * argv[]) {
    LI("Hello, SClient!!!");
    MsgServerClient::PrintVersion();

    if (argc <= 1) {
        std::cout << "Error: Please usage:$0 {userid} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    std::string logpath("./client_log");
    logpath.append(argv[1]);
    //L_Init(2, logpath.c_str());
    L_Init(2, NULL);

    MsgServerClient::Initialize(1024);
    MsgServerClient* pSClient = MsgServerClient::Inst();
    int res = pSClient->Start(argv[1], "192.168.7.207", 6630);
    int test = 0;
    if (res != 0) {
        LI("MsgServerClient start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 120) {
    while (1) {
        pSClient->DoTick();
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pSClient->Stop();
    MsgServerClient::DeInitialize();
    L_Deinit();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
