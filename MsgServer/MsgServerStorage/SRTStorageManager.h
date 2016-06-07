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
#include "MsgServer/proto/storage_msg.pb.h"

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
        m_SessCount = 0;
        m_LastOneUsed = m_TransferSessions.end();
        std::string recvfile("StorageServer");
        recvfile.append("-recv");
        {
            m_RecvFile = fopen(recvfile.c_str(), "w");
            if (!m_RecvFile)
            {
                std::cout << "fopen file for user " << recvfile << " failed" << std::endl;
                Assert(false);
            }
        }
        std::string sendfile("StorageServer");
        sendfile.append("-send");
        {
            m_SendFile = fopen(sendfile.c_str(), "w");
            if (!m_SendFile)
            {
                std::cout << "fopen file for user " << sendfile << " failed" << std::endl;
                Assert(false);
            }
        }
    }

    bool UninManager()
    {
        if (m_SendFile)
        {
            fclose(m_SendFile);
            m_SendFile = nullptr;
        }
        if (m_RecvFile)
        {
            fclose(m_RecvFile);
            m_RecvFile = nullptr;
        }
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

    void AddTransferSession(SRTTransferSession* session)
    {
        m_TransferSessions.push_back(session);
    }
    void DelTransferSession(SRTTransferSession* session)
    {
        m_TransferSessions.erase(std::find(m_TransferSessions.begin(), m_TransferSessions.end(), session));
    }
    SRTTransferSession* PickupTransferSession()
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
    SRTStorageManager()
    {

    }

    ~SRTStorageManager()
    {

    }
private:
    std::vector<std::string>            m_RedisHosts;
    std::vector<SRTStorageRedis*>      m_RedisGroups;
    FILE*                               m_SendFile;
    FILE*                               m_RecvFile;
    long long                           m_SessCount;
    std::list<SRTTransferSession*>      m_TransferSessions;
    std::list<SRTTransferSession*>::const_iterator    m_LastOneUsed;
};

#endif /* defined(__MsgServerStorage__SRTStorageManager__) */
