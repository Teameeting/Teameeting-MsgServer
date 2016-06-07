//
//  SRTSequenceManager.h
//  MsgServerSequence
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerSequence__SRTSequenceManager__
#define __MsgServerSequence__SRTSequenceManager__

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
#include "SRTSequenceGenerator.h"
#include "SRTSequenceRedis.h"

#define DEF_PROTO 1
#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/meet_msg.pb.h"
#include "MsgServer/proto/meet_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"
#include "MsgServer/proto/storage_msg.pb.h"

#define HR_USERID       "hr_userid"
#define HR_CONNECTORID  "hr_connectorid"

class SRTTransferSession;

class SRTSequenceManager : public RTSingleton< SRTSequenceManager >{
    friend class RTSingleton< SRTSequenceManager >;
public:


    bool RecvRequestCounter();
    bool SendResponseCounter();

    void AddRedisServer(SRTSequenceRedis* redis)
    {
        m_RedisGroups.push_back(redis);
    }

    bool InitManager()
    {
        m_SessCount = 0;
        m_LastOneUsed = m_TransferSessions.end();
        std::string recvfile("SequenceServer");
        recvfile.append("-recv");
        {
            m_RecvFile = fopen(recvfile.c_str(), "w");
            if (!m_RecvFile)
            {
                std::cout << "fopen file for user " << recvfile << " failed" << std::endl;
                Assert(false);
            }
        }
        std::string sendfile("SequenceServer");
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
        std::vector<SRTSequenceRedis*>::iterator it = m_RedisGroups.begin();
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
        std::vector<SRTSequenceRedis*>::iterator it = m_RedisGroups.begin();
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
    SRTSequenceManager()
    {

    }

    ~SRTSequenceManager()
    {

    }
private:
    std::vector<std::string>            m_RedisHosts;
    std::vector<SRTSequenceRedis*>      m_RedisGroups;
    FILE*                               m_SendFile;
    FILE*                               m_RecvFile;
    long long                           m_SessCount;
    std::list<SRTTransferSession*>      m_TransferSessions;
    std::list<SRTTransferSession*>::const_iterator    m_LastOneUsed;
};

#endif /* defined(__MsgServerSequence__SRTSequenceManager__) */
