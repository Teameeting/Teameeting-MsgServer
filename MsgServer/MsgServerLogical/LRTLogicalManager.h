//
//  LRTLogicalManager.h
//  MsgServerLogical
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerLogical__LRTLogicalManager__
#define __MsgServerLogical__LRTLogicalManager__

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

class LRTTransferSession;

class LRTLogicalManager : public RTSingleton< LRTLogicalManager >{
    friend class RTSingleton< LRTLogicalManager >;
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
    LRTLogicalManager()
    {

    }

    ~LRTLogicalManager()
    {

    }
private:
    std::vector<std::string>            m_RedisHosts;
    std::list<LRTTransferSession*>      m_TransferSessions;
};

#endif /* defined(__MsgServerLogical__LRTLogicalManager__) */
