//
//  ClientManager.h
//  SeqnClient
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __SeqnClient__ClientManager__
#define __SeqnClient__ClientManager__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "ClientSession.h"
#include "RTSingleton.h"

#define DEF_PROTO 1
#include "client_common/proto/storage_msg.pb.h"
#include "client_common/proto/storage_msg_type.pb.h"
#include "client_common/proto/sys_msg.pb.h"
#include "client_common/proto/sys_msg_type.pb.h"
#include "client_common/proto/common_msg.pb.h"

class ClientManager : public RTSingleton< ClientManager >{
    friend class RTSingleton< ClientManager >;
public:

    void InitClient(const char* pGroupid, const char* pUserid, const char* pIp, unsigned int port);
    void UninClient();

    void AddGroup(const std::string& groupid);
    void SendGroupMsg();

protected:
    ClientManager() {}
    ~ClientManager() {}
private:
    ClientSession*   mClientSession;
    std::ifstream    mIfs;

};

#endif /* defined(__SeqnClient__ClientManager__) */
