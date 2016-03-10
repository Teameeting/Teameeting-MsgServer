//
//  main.cpp
//  MsgServerMeeting
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "config_parser.h"
#include "rtklog.h"
#include "MRTMeeting.h"
#include "StrPtrLen.h"


#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    LI("Hello, Meeting!!!");
    MRTMeeting::PrintVersion();

    ConfigSet conf;
    if (argc > 1) {
        conf.LoadFromFile(argv[1]);
    } else {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int debugEnable = conf.GetIntVal("global", "debug", 0);
    std::string strLocalIp("");
    std::string strGlobalIp("");
    int nConnPort = conf.GetIntVal("global", "accept_conn_port", 6620);
    int nDispPort = conf.GetIntVal("global", "accept_disp_port", 6640);
    if (argc > 2) {
         strLocalIp  = argv[2];
         strGlobalIp = argv[2];
    } else {
        strLocalIp  = conf.GetValue("global", "int_ip", "127.0.0.1");
        strGlobalIp = conf.GetValue("global", "ext_ip");
    }
    if (strLocalIp.length()==0 || strGlobalIp.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }
    std::string strHttpIp = conf.GetValue("meeting", "http_ip");
    int nHttpPort = conf.GetIntVal("meeting", "accept_http_port", 8055);

    int log_level = conf.GetIntVal("log", "level", 5);
    std::string strLogPath = conf.GetValue("log", "path");
    if (log_level < 0 || log_level > 5) {
        std::cout << "Error: Log level=" << log_level << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    L_Init(5, NULL);
    MRTMeeting::Initialize(1024);
    MRTMeeting* pMeeting = MRTMeeting::Inst();
    pMeeting->Start(strLocalIp.c_str(), nConnPort, strLocalIp.c_str(), nDispPort, strHttpIp.c_str(), nHttpPort);
    while (true) {
        pMeeting->DoTick();
        sleep(1);
        //break;
    }
    MRTMeeting::DeInitialize();
    L_Deinit();
    return 0;
}
