//
//  SRTStorageRedis.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTStorageRedis.h"
#include "SRTRedisGroup.h"
#include "MsgServer/proto/storage_msg.pb.h"
#include "MsgServer/proto/storage_msg_type.pb.h"
#include "OSThread.h"

static int g_push_event_counter = 0;
static OSMutex      g_push_event_mutex;

static unsigned int APHash(const char *str) {
    unsigned int hash = 0;
    int i;
    for (i=0; *str; i++) {
        if ((i&  1) == 0) {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        } else {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
    return (hash&  0x7FFFFFFF);
}

enum {
 CACHE_TYPE_1,
 CACHE_TYPE_2,
 CACHE_TYPE_MAX,
};




void SRTStorageRedis::Init(SRTRedisGroup* group, const std::string& ip, int port)
{
    //LI("SRTStorageRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);
    m_Ip = ip;
    m_Port = port;
    m_RedisGroup = group;

    m_RedisList = new RedisNode;
    m_RedisList->dbindex = 0;
    m_RedisList->host = m_Ip.c_str();
    m_RedisList->port = m_Port;
    m_RedisList->passwd = "";
    m_RedisList->poolsize = 8;
    m_RedisList->timeout = 5;
    m_RedisList->role = 0;

    m_xRedisClient.Init(CACHE_TYPE_MAX);
    m_xRedisClient.ConnectRedisCache(m_RedisList, 1, CACHE_TYPE_1);
    m_redisDBIdx = new RedisDBIdx(&m_xRedisClient);
}

void SRTStorageRedis::Unin()
{
    //LI("SRTStorageRedis::Unin was called, host:%s\n", GetHostForTest().c_str());
    m_RedisGroup = nullptr;
    if (m_RedisList)
    {
        delete m_RedisList;
        m_RedisList = nullptr;
    }
    if (m_redisDBIdx)
    {
         delete m_redisDBIdx;
         m_redisDBIdx = nullptr;
    }
    //this->DisConn();
}

// from RTEventLooper
void SRTStorageRedis::OnPostEvent(const char*pData, int nSize)
{

}

void SRTStorageRedis::OnWakeupEvent(const void*pData, int nSize)
{

}

void SRTStorageRedis::OnPushEvent(const char*pData, int nSize)
{
    //printf("OnPushEvent...\n");
    if (!pData || nSize <=0)
    {
        printf("SRTStorageRedis::OnPushEvent params erroooooooooooooooooooooooor\n");
        return;
    }
    std::string str(pData, nSize);
    m_RecvStoreMsg.ParseFromString(str);
    // 1 write, 2 read
    printf("SRTStorageRedis::OnPushEvent msg flag:%d\n", m_RecvStoreMsg.mflag());
    if (m_RecvStoreMsg.mflag() == pms::EStorageType::TWRITE)
    {
        char key[1024] = {'\0'};
        sprintf(key, "%s:%s", m_RecvStoreMsg.userid().c_str(), m_RecvStoreMsg.msgid().c_str());
        m_redisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

        bool ok = m_xRedisClient.set(*m_redisDBIdx, key, m_RecvStoreMsg.content().c_str());
        printf("SRTStorageRedis::OnPushEvent ok:%d, key:%s\n", ok, key);
        {
            OSMutexLocker locker(&g_push_event_mutex);
            char num[8] = {0};
            //printf("SRTStorageRedis::OnPushEvent key:%s, g_push_event_counter:%d\n\n", key, ++g_push_event_counter);
            sprintf(num, "%d", ++g_push_event_counter);
            m_RecvStoreMsg.set_result(20);
            m_RecvStoreMsg.mutable_content()->append(num);
        }
        if (m_RedisGroup)
        {
            m_RedisGroup->PostData(m_RecvStoreMsg.SerializeAsString());
        }
    } else if (m_RecvStoreMsg.mflag() == pms::EStorageType::TREAD)
    {
        std::string str("");
        char key[1024] = {'\0'};
        sprintf(key, "%s:%s", m_RecvStoreMsg.userid().c_str(), m_RecvStoreMsg.msgid().c_str());
        m_redisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

        bool ok = m_xRedisClient.get(*m_redisDBIdx, key, str);
        m_RecvStoreMsg.set_result(30);
        *m_RecvStoreMsg.mutable_content() = str;
        if (m_RedisGroup)
        {
            m_RedisGroup->PostData(m_RecvStoreMsg.SerializeAsString());
        }
    }
    m_RecvStoreMsg.Clear();
}

void SRTStorageRedis::OnTickEvent(const void*pData, int nSize)
{

}

bool SRTStorageRedis::IsTheSameRedis(const std::string& host, int port)
{
    return ((m_Ip.compare(host)==0) && (m_Port==port));
}
///////////////////////////////////////////////////////
