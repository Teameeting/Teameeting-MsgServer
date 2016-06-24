//
//  SRTSequenceRedis.cpp
//  dyncRTSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTSequenceRedis.h"
#include "SRTRedisManager.h"

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

void SRTSequenceRedis::Init(SRTRedisManager* manager, const std::string& ip, int port)
{
    //LI("SRTSequenceRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);

    m_Ip = ip;
    m_Port = port;
    m_RedisManager = manager;

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
    m_RedisDBIdx = new RedisDBIdx(&m_xRedisClient);

    WriteResponse.connect(m_RedisManager, &SRTRedisManager::OnWriteSeqn);
    ReadResponse.connect(m_RedisManager, &SRTRedisManager::OnReadSeqn);
}

void SRTSequenceRedis::Unin()
{
    //LI("SRTSequenceRedis::Unin was called, host:%s\n", GetHostForTest().c_str());
    ReadResponse.disconnect(m_RedisManager);
    WriteResponse.disconnect(m_RedisManager);
    m_RedisManager = nullptr;
    if (m_RedisList)
    {
        delete m_RedisList;
        m_RedisList = nullptr;
    }
    if (m_RedisDBIdx)
    {
         delete m_RedisDBIdx;
         m_RedisDBIdx = nullptr;
    }
}

// from RTEventLooper
// post for read
void SRTSequenceRedis::OnPostEvent(const char*pData, int nSize)
{
    if (!pData || nSize<=0) return;
    std::string str(pData, nSize);
    pms::StorageMsg request;
    request.ParseFromString(str);
    {
        int64 seq = 0;
        std::string str("");
        char key[1024] = {'\0'};
        sprintf(key, "%s", request.storeid().c_str());
        printf("SRTSequenceRedis::OnPostEvent msgid:%s, key:%s\n", request.msgid().c_str(), key);
        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
        bool ok = m_xRedisClient.get(*m_RedisDBIdx, key, str);

        seq = std::stoll(str);
        printf("SRTSequenceRedis::OnPostEvent read seqn:%s, seq:%lld\n", str.c_str(), seq);
        printf("OnPostEvent GetSeq is:%lld\n", seq);
        ReadResponse(request, seq);
    }
}

// push for write
void SRTSequenceRedis::OnPushEvent(const char*pData, int nSize)
{
    if (!pData || nSize<=0) return;
    std::string str(pData, nSize);
    pms::StorageMsg request;
    request.ParseFromString(str);
    {
        int64_t seq = 0;
        char key[1024] = {'\0'};
        sprintf(key, "%s", request.storeid().c_str());
        printf("SRTSequenceRedis::OnPushEvent msgid:%s, key:%s\n", request.msgid().c_str(), key);
        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
        bool ok = m_xRedisClient.incr(*m_RedisDBIdx, key, seq);

        printf("OnPushEvent IncrSeq is:%ld\n", seq);
        WriteResponse(request, seq);
    }
}

///////////////////////////////////////////////////////
