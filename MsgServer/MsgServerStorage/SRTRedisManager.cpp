//
//  SRTRedisManager.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTRedisManager.h"
#include "SRTStorageManager.h"
#include "SRTTransferSession.h"
#include "SRTResponseCollection.h"

#define SESSION_TIMEOUT (90*1000)
#define REDIS_GROUP_CLIENT (10)
#define PACKED_MSG_ONCE_NUM (10)
#define PACKED_MSG_POOL_SIZE (10)

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
    ListZero(&m_SeqnResp2Send);
    for(int i=0;i<PACKED_MSG_POOL_SIZE;++i)
    {
        pms::PackedStoreMsg* psm = new pms::PackedStoreMsg;
        for(int j=0;j<PACKED_MSG_ONCE_NUM;++j)
        {
            psm->add_msgs();
        }
        m_PackedStoreMsgs.push_back(psm);
    }
    //TODO:
    // if redis crash, m_RedisNum should be notifyed
    SRTStorageManager::Instance().GetRedisHosts(&m_RedisHosts);
    m_RedisNum = m_RedisHosts.size();
    for(std::vector<std::string>::iterator it=m_RedisHosts.begin();it!=m_RedisHosts.end();++it)
    {
        std::string s = *it;
        char ip[16] = {0};
        int port = 0;
        sscanf(s.c_str(), "%s %d", ip, &port);
        printf("Generator Init ip:%s, port:%d\n", ip, port);
        RedisGroup* redisGroup = new RedisGroup;
        redisGroup->ip = ip;
        redisGroup->port = port;
        if (strlen(ip)>0 && port >0)
        {
            for (int i=0;i<REDIS_GROUP_CLIENT;++i)
            {
                SRTStorageRedis* redis = new SRTStorageRedis();;
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
    for(auto x : m_PackedStoreMsgs)
    {
        delete x;
    }
    m_PackedStoreMsgs.clear();
    ListEmpty(&m_SeqnResp2Send);
    for(auto x : m_ResponseCollections)
    {
        delete x.second;
    }
    m_ResponseCollections.clear();
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
void SRTRedisManager::PushRedisRequest(const std::string& request)
{
    //LI("SRTRedisManager::PushRedisRequest was called\n");
    //std::string um("");
    //um.append(request.userid()).append(":").append(request.msgid());
    for(auto x : m_RedisGroupMgr)
    {
        LoopupForRedis((RedisGroup*)x.second)->PushData(request.c_str(), request.length());
    }
}

SRTStorageRedis* SRTRedisManager::LoopupForRedis(RedisGroup* group)
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
void SRTRedisManager::OnRequestSeqn(const std::string& userid, const std::string& msgid, long long seqn)
{
    //LI("SRTRedisManager::OnRequestSeqn userid:%s, msgid:%s, seqn:%lld\n", userid.c_str(), msgid.c_str(), seqn);
    OSMutexLocker locker(&m_MutexCollection);
    std::unordered_map<std::string, SRTResponseCollection*>::const_iterator cit = m_ResponseCollections.find(userid);
    if (cit != m_ResponseCollections.end())
    {
        //LI("OnRequestSeqn userid find out:%s\n", userid.c_str());
        cit->second->AddResponse(msgid, seqn);
    } else {
        //LI("OnRequestSeqn userid not find out:%s\n", userid.c_str());
        m_ResponseCollections.insert(make_pair(userid, new SRTResponseCollection(this, m_RedisNum, userid, msgid, seqn)));
    }
}


/**
 * after collection all the seqn with same userid and msgid
 * this method will be invoked by MsgSeqn
 *
 * this function just push to list and fire event
 *
 */
void SRTRedisManager::OnAddAndCheckSeqn(const std::string& msg)
{
    //LI("SRTRedisManager::OnAddAndCheckSeqn userid:%s, msgid:%s, seqn:%lld\n", userid.c_str(), msgid.c_str(), seqn);
#if 1
    {
        char* ptr = (char*)malloc(sizeof(char)*((int)msg.length()+1));
        memcpy(ptr, msg.c_str(), (int)msg.length());
        ptr[(int)msg.length()] = '\0';
        {
            OSMutexLocker locker(&m_Mutex2Send);
            m_PackedCounter++;
            ListAppend(&m_SeqnResp2Send, ptr, (int)msg.length());
        }
    }
    if (m_PackedCounter==PACKED_MSG_ONCE_NUM)
    {
        m_PackedCounter = 0;
        this->Signal(kIdleEvent);
    }
#else
    m_PackedStoreMsgs.
#endif

}

// from RTEventLooper
void SRTRedisManager::OnPostEvent(const char*pData, int nSize)
{

}

void SRTRedisManager::OnWakeupEvent(const char*pData, int nSize)
{

}


void SRTRedisManager::OnPushEvent(const char*pData, int nSize)
{

}

void SRTRedisManager::OnTickEvent(const void*pData, int nSize)
{
     ListElement *elem = NULL;
    pms::PackedStoreMsg * pit = new pms::PackedStoreMsg;
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if((elem = m_SeqnResp2Send.first) != NULL)
        {
            std::string s((const char*)elem->content, elem->size);
            pms::StorageMsg* tit = pit->add_msgs();
            tit->ParseFromString(s);
            {
                OSMutexLocker locker(&m_Mutex2Send);
                ListRemoveHead(&m_SeqnResp2Send);
            }
        }
    }
    std::string ss = pit->SerializeAsString();
    SRTStorageManager::Instance().PickupTransferSession()->SendTransferData(ss.c_str(), (int)ss.length());
    delete pit;
    pit = nullptr;
}


///////////////////////////////////////////////////////
