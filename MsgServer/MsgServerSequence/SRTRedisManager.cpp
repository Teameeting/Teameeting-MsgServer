//
//  SRTRedisManager.cpp
//  dyncRTSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTRedisManager.h"
#include "SRTSequenceManager.h"
#include "SRTTransferSession.h"
#include "SRTResponseCollection.h"

#define SESSION_TIMEOUT (90*1000)
#define REDIS_GROUP_CLIENT (10)
#define PACKED_MSG_ONCE_NUM (10)

SRTRedisManager::SRTRedisManager()
    : m_LastUpdateTime(0)
{
}

SRTRedisManager::~SRTRedisManager()
{
}

void SRTRedisManager::Init(SRTTransferSession* sess)
{
    m_Session = sess;
    m_PackedCounter = 0;
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_WritePackedMsg.add_msgs();
        m_ReadPackedMsg.add_msgs();
    }
    //TODO:
    // if redis crash, m_RedisNum should be notifyed
    SRTSequenceManager::Instance().GetRedisHosts(&m_RedisHosts);
    m_RedisNum = m_RedisHosts.size();
    for(std::vector<std::string>::iterator it=m_RedisHosts.begin();it!=m_RedisHosts.end();++it)
    {
        std::string s = *it;
        char ip[16] = {0};
        int port = 0;
        sscanf(s.c_str(), "%s %d", ip, &port);
        printf("SequenceGenerator Redis Init ip:%s, port:%d\n", ip, port);
        RedisGroup* redisGroup = new RedisGroup;
        redisGroup->ip = ip;
        redisGroup->port = port;
        if (strlen(ip)>0 && port >0)
        {
            for (int i=0;i<REDIS_GROUP_CLIENT;++i)
            {
                SRTSequenceRedis* redis = new SRTSequenceRedis();;
                redis->Init(this, ip, port);
                redisGroup->redises.push_back(redis);
            }
        } else {
            Assert(false);
        }
        m_RedisGroupMgr.insert(make_pair(s, redisGroup));
    }
}

void SRTRedisManager::Unin()
{
    m_Session = nullptr;

    while(!m_SeqnResp4Read.empty())
    {
        m_SeqnResp4Read.pop();
    }
    while(!m_SeqnResp4Write.empty())
    {
        m_SeqnResp4Write.pop();
    }

    for(auto x : m_ReadResponseCollections)
    {
        delete x.second;
    }
    m_ReadResponseCollections.clear();

    for(auto x : m_WriteResponseCollections)
    {
        delete x.second;
    }
    m_WriteResponseCollections.clear();

    for(auto x : m_RedisGroupMgr)
    {
        auto redisGroup = x.second;
        for (auto y : redisGroup->redises)
        {
            y->Unin();
            delete y;
            y = nullptr;
        }
        delete redisGroup;
        redisGroup = nullptr;
    }
    m_RedisGroupMgr.clear();
}

/**
 * all the reqeust invoke this method
 * send to random redis client by redis ip
 *
 */

// post for read
void SRTRedisManager::PostRedisRequest(const std::string& request)
{
    LI("SRTRedisManager::PostRedisRequest was called\n");
    for(auto x : m_RedisGroupMgr)
    {
        LoopupForRedis((RedisGroup*)x.second)->PostData(request.c_str(), request.length());
    }
}

// push for write
void SRTRedisManager::PushRedisRequest(const std::string& request)
{
    LI("SRTRedisManager::PushRedisRequest was called\n");
    for(auto x : m_RedisGroupMgr)
    {
        LoopupForRedis((RedisGroup*)x.second)->PushData(request.c_str(), request.length());
    }
}

SRTSequenceRedis* SRTRedisManager::LoopupForRedis(RedisGroup* group)
{
    return group->redises[0];
}

/**
 * after redis client get seqn
 * this method will be invoked by redis client
 *
 * this function store all the seqn by same userid and msgid
 *
 */
void SRTRedisManager::OnWriteSeqn(const pms::StorageMsg& request, long long seqn)
{
    LI("SRTRedisManager::OnWriteSeqn userid:%s, msgid:%s, seqn:%lld\n", request.userid().c_str(), request.msgid().c_str(), seqn);
    OSMutexLocker locker(&m_MutexWriteCollection);
    std::unordered_map<std::string, SRTResponseCollection*>::const_iterator cit = m_WriteResponseCollections.find(request.userid());
    if (cit != m_WriteResponseCollections.end())
    {
        LI("OnWriteSeqn userid find out:%s\n", request.userid().c_str());
        cit->second->AddResponse(request, seqn);
    } else {
        LI("OnWriteSeqn userid not find out:%s\n", request.userid().c_str());
        m_WriteResponseCollections.insert(make_pair(request.userid(), new SRTResponseCollection(this, REQUEST_TYPE_WRITE, m_RedisNum, request, seqn)));
    }
}


/**
 * after collection all the seqn with same userid and msgid
 * this method will be invoked by MsgSeqn
 *
 * this function just push to list and fire event
 *
 */
void SRTRedisManager::OnAddAndCheckWrite(const std::string& msg)
{
    {
        OSMutexLocker locker(&m_Mutex4Write);
        m_SeqnResp4Write.push(msg);
    }
    this->Signal(kIdleEvent);
}

/**
 * after redis client get seqn
 * this method will be invoked by redis client
 *
 * this function store all the seqn by same userid and msgid
 *
 */
void SRTRedisManager::OnReadSeqn(const pms::StorageMsg& request, long long seqn)
{
    LI("SRTRedisManager::OnReadSeqn userid:%s, msgid:%s, seqn:%lld\n", request.userid().c_str(), request.msgid().c_str(), seqn);
    OSMutexLocker locker(&m_MutexReadCollection);
    std::unordered_map<std::string, SRTResponseCollection*>::const_iterator cit = m_ReadResponseCollections.find(request.userid());
    if (cit != m_ReadResponseCollections.end())
    {
        LI("OnReadSeqn userid find out:%s\n", request.userid().c_str());
        cit->second->AddResponse(request, seqn);
    } else {
        LI("OnReadSeqn userid not find out:%s\n", request.userid().c_str());
        m_ReadResponseCollections.insert(make_pair(request.userid(), new SRTResponseCollection(this, REQUEST_TYPE_READ, m_RedisNum, request, seqn)));
    }
}


/**
 * after collection all the seqn with same userid and msgid
 * this method will be invoked by MsgSeqn
 *
 * this function just push to list and fire event
 *
 */
void SRTRedisManager::OnAddAndCheckRead(const std::string& msg)
{
    {
        OSMutexLocker locker(&m_Mutex4Read);
        m_SeqnResp4Read.push(msg);
    }
    this->Signal(kWakeupEvent);
}

// from RTEventLooper

// this is for read
void SRTRedisManager::OnWakeupEvent(const void*pData, int nSize)
{
    if (m_SeqnResp4Read.size()==0) return;
    bool hasData = false;
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_SeqnResp4Read.size()>0)
        {
            hasData = true;
            m_ReadPackedMsg.mutable_msgs(i)->ParseFromString(m_SeqnResp4Read.front());
            {
                OSMutexLocker locker(&m_Mutex4Read);
                m_SeqnResp4Read.pop();
            }
        } else {
            break;
        }
    }
    if (hasData)
    {
        if (m_Session && m_Session->IsLiveSession())
        {
            printf("SRTRedisManager::OnWakeupEvent TREQUEST\n");

            pms::RelayMsg rmsg;
            rmsg.set_svr_cmds(pms::EServerCmd::CSYNCSEQN);
            rmsg.set_content(m_ReadPackedMsg.SerializeAsString());

            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TREAD_RESPONSE);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(rmsg.SerializeAsString());

            m_Session->SendTransferData(tmsg.SerializeAsString());
        }
        for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
             m_ReadPackedMsg.mutable_msgs(i)->Clear();
        }
    }
    if (m_SeqnResp4Read.size()>0)
    {
        this->Signal(kWakeupEvent);
    }
}

// this is for write
void SRTRedisManager::OnTickEvent(const void*pData, int nSize)
{
    if (m_SeqnResp4Write.size()==0) return;
    bool hasData = false;
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_SeqnResp4Write.size()>0)
        {
            hasData = true;
            m_WritePackedMsg.mutable_msgs(i)->ParseFromString(m_SeqnResp4Write.front());
            {
                OSMutexLocker locker(&m_Mutex4Write);
                m_SeqnResp4Write.pop();
            }
        } else {
            break;
        }
    }
    if (hasData)
    {
        if (m_Session && m_Session->IsLiveSession())
        {
            pms::RelayMsg rmsg;
            rmsg.set_svr_cmds(pms::EServerCmd::CNEWMSGSEQN);
            rmsg.set_content(m_WritePackedMsg.SerializeAsString());

            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TWRITE_RESPONSE);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(rmsg.SerializeAsString());

            m_Session->SendTransferData(tmsg.SerializeAsString());
        }
        for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
             m_WritePackedMsg.mutable_msgs(i)->Clear();
        }
    }
    if (m_SeqnResp4Write.size()>0)
    {
        this->Signal(kIdleEvent);
    }
}


///////////////////////////////////////////////////////
