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
        m_SessCount = 0;
        m_LastOneUsed = m_TransferSessions.end();
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

    void AddTransferSession(MRTTransferSession* session)
    {
        m_TransferSessions.push_back(session);
    }
    void DelTransferSession(MRTTransferSession* session)
    {
        m_TransferSessions.erase(std::find(m_TransferSessions.begin(), m_TransferSessions.end(), session));
    }
    MRTTransferSession* PickupTransferSession()
    {
        if (m_LastOneUsed == m_TransferSessions.cend())
        {
            m_LastOneUsed = m_TransferSessions.cbegin();
        }
        return *m_LastOneUsed++;
    }

    bool    SignalKill();
    bool    ClearAll();
    void    SessionCounter() { ++m_SessCount; }
protected:
    MRTModuleManager()
    {

    }

    ~MRTModuleManager()
    {

    }
private:
    std::vector<std::string>            m_RedisHosts;
    long long                           m_SessCount;
    std::list<MRTTransferSession*>      m_TransferSessions;
    std::list<MRTTransferSession*>::const_iterator    m_LastOneUsed;
};

#endif /* defined(__MsgServerModule__MRTModuleManager__) */
