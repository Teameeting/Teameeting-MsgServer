//
//  MRTModuleManager.h
//  MsgServerModule
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerModule__MRTModuleManager__
#define __MsgServerModule__MRTModuleManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include "RTType.h"
#include "RTSingleton.h"

#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

class MRTTransferSession;

class MRTModuleManager : public RTSingleton< MRTModuleManager >{
    friend class RTSingleton< MRTModuleManager >;
public:

    bool RecvRequestCounter();
    bool SendResponseCounter();

    bool InitManager()
    {

    }

    bool UninManager()
    {
        m_RedisHosts.clear();
    }

    void PushRedisHosts(std::string host)
    {
        m_RedisHosts.push_back(host);
    }

    void GetRedisHosts(std::vector<std::string>* hosts)
    {
         for(std::vector<std::string>::iterator it=m_RedisHosts.begin();it!=m_RedisHosts.end();++it)
         {
             hosts->push_back(*it);
         }
    }

    bool    SignalKill();
    bool    ClearAll();
protected:
    MRTModuleManager()
    {

    }

    ~MRTModuleManager()
    {

    }
private:
    std::vector<std::string>            m_RedisHosts;
    std::list<MRTTransferSession*>      m_TransferSessions;
};

#endif /* defined(__MsgServerModule__MRTModuleManager__) */
