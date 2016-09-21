//
//  SRTStorageManager.h
//  MsgServerStorage
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTStorageManager__
#define __MsgServerStorage__SRTStorageManager__

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
#include "SRTStorageRedis.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

#define HR_USERID       "hr_userid"
#define HR_CONNECTORID  "hr_connectorid"

class SRTTransferSession;

class SRTStorageManager : public RTSingleton< SRTStorageManager >{
    friend class RTSingleton< SRTStorageManager >;
public:


    bool RecvRequestCounter();
    bool SendResponseCounter();

    void AddRedisServer(SRTStorageRedis* redis)
    {
        m_RedisGroups.push_back(redis);
    }

    bool InitManager()
    {
    }

    bool UninManager()
    {
        m_RedisHosts.clear();
    }

    void DelRedisServer(const std::string& host, int port)
    {
        std::vector<SRTStorageRedis*>::iterator it = m_RedisGroups.begin();
        while(it != m_RedisGroups.end())
        {
            if((*it)->IsTheSameRedis(host, port))
            {
                delete *it;
                m_RedisGroups.erase(it);
                break;
            }
            it++;
        }
    }

    void ClearRedisServer()
    {
        std::vector<SRTStorageRedis*>::iterator it = m_RedisGroups.begin();
        while(it != m_RedisGroups.end())
        {
            delete *it;
            m_RedisGroups.erase(it);
            it++;
        }
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
    SRTStorageManager()
    {

    }

    ~SRTStorageManager()
    {

    }
private:
    std::vector<std::string>            m_RedisHosts;
    std::vector<SRTStorageRedis*>       m_RedisGroups;
    std::list<SRTTransferSession*>      m_TransferSessions;
};

#endif /* defined(__MsgServerStorage__SRTStorageManager__) */
