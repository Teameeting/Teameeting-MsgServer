//
//  main.cpp
//  MsgServerConnector
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include "config_parser.h"
#include "rtklog.h"
#include "CRTConnector.h"

#ifndef _DEBUG
#define _DEBUG 0
#endif

int main(int argc, const char * argv[]) {
    LI("Hello, Connector!!!");
    CRTConnector::PrintVersion();

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
    int nWebConPort = conf.GetIntVal("global", "listen_webcon_port", 6610);
    int nModulePort = conf.GetIntVal("global", "listen_module_port", 6620);
    int nCliConPort = conf.GetIntVal("global", "listen_clicon_port", 6630);
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
    std::string strHttpIp = conf.GetValue("connector", "http_ip");
    int nHttpPort = conf.GetIntVal("connector", "listen_http_port", 8055);

    int log_level = conf.GetIntVal("log", "level", 5);
    std::string strLogPath = conf.GetValue("log", "path");
    if (log_level < 0 || log_level > 5) {
        std::cout << "Error: Log level=" << log_level << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    L_Init(0, NULL);
    CRTConnector::Initialize(1024);
    CRTConnector* pConnector = CRTConnector::Inst();
    pConnector->Start(strGlobalIp.c_str(), nWebConPort, strGlobalIp.c_str(), nModulePort, strGlobalIp.c_str(), nCliConPort, strHttpIp.c_str(), nHttpPort);
    while (true) {
        pConnector->DoTick();
        sleep(1);
        //break;
    }
    CRTConnector::DeInitialize();
    L_Deinit();
    return 0;
}
