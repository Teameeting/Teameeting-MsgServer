//
//  main.cpp
//  MsgServerPusher
//
//  Created by hp on 1/16/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "rtklog.h"
#include "PRTPusher.h"
#include "RTZKClient.hpp"
#include <google/protobuf/message.h>

#include "IosPusher.h"

#ifndef _TEST_
#define _TEST_ 1
#endif

int main(int argc, const char * argv[]) {
    printf("Hello, Pusher!!!\n");
    PRTPusher::PrintVersion();

    if (argc <= 2) {
        std::cout << "Error: Please usage:$0 {conf_path} " << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

#if 0
#if _TEST_
    if (RTZKClient::Instance().InitOnly(argv[1])!=0) {
#else
    if (RTZKClient::Instance().InitZKClient(argv[1])!=0) {
#endif
        std::cout << "Please check the config file ..." << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int level = RTZKClient::Instance().GetServerConfig().Level;
    std::string logpath = RTZKClient::Instance().GetServerConfig().LogPath;
    if (logpath.empty())
        L_Init(level, NULL);
    else
        L_Init(level, logpath.c_str());
#endif

#if 0
    L_Init(0, NULL);
#else
    L_Init(0, "./logpusher.log");
#endif

    MsConfigParser conf;
    conf.LoadFromFile(argv[2]);

    int debug = conf.GetIntVal("global", "debug", 1);
    std::string strCertDir("");
    std::string strP12Path("");
    std::string strPassPath("");
    if (debug)
    {
        strCertDir = conf.GetValue("filepath", "cert_dir_debug", ".");
        strP12Path = conf.GetValue("filepath", "p12_path_debug", "cert/com.avcon.BuddhismLiving/dev/com_avcon_BuddhismLiving_dev.p12");
        strPassPath = conf.GetValue("filepath", "pass_path_debug", "cert/com.avcon.buddhismliving/dev/pass");
    } else {
        strCertDir = conf.GetValue("filepath", "cert_dir_release", "/usr/local/dync/msgserver/pusher/bin");
        strP12Path = conf.GetValue("filepath", "p12_path_release", "cert/com.avcon.BuddhismLiving/rel/com_avcon_BuddhismLiving_dev.p12");
        strPassPath = conf.GetValue("filepath", "pass_path_release", "cert/com.avcon.buddhismliving/rel/pass");
    }
    if (strCertDir.length()==0 || strP12Path.length()==0 || strPassPath.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    std::string pass_path = strCertDir + "/" + strPassPath;
    std::cout << "push notification pass path:" << pass_path << std::endl;
    std::cout << "push notification p12 path:" << strP12Path << std::endl;
    std::ifstream in;
    in.open(pass_path);
    if (!in.is_open())
    {
        std::cout << "Error opening file:" << pass_path << std::endl;
        exit(0);
    }
    char buffer[129] = {0};
    in.getline(buffer, 128);
    in.close();
    std::cout << "pass is:" << buffer << std::endl;

    if (!IosPusher::Instance().InitPusher(strCertDir.c_str(), strP12Path.c_str(), buffer)) exit(0);
    if (!IosPusher::Instance().ConnectApns()) exit(0);
    if (!IosPusher::Instance().InitConfig()) exit(0);

    printf("Connect to Apns Ok!!!\n");


    PRTPusher::Initialize(1024);
    PRTPusher* pPusher = PRTPusher::Inst();
    //int test = 0;
    int res = pPusher->Start(conf);
    if (res != 0) {
        LI("PRTPusher start failed and goto exit, res:%d\n", res);
        goto EXIT;
    }
    //while (test++ < 30) {
    while (1) {
        pPusher->DoTick();
        sleep(1);
        //break;
    }
        sleep(1);
EXIT:
    pPusher->Stop();
    PRTPusher::DeInitialize();
    IosPusher::Instance().UninPusher();
    L_Deinit();
    //RTZKClient::Instance().Unin();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
