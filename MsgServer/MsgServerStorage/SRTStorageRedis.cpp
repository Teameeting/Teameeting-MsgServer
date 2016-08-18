//
//  SRTStorageRedis.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTStorageRedis.h"
#include "SRTRedisGroup.h"
#include "OSThread.h"

#define MAX_MESSAGE_EXPIRE_TIME (7*24*60*60)

#define Err_Redis_Ok                     (0)
#define Err_Redis_Not_Exist              (-1)
#define Err_Redis_Exist                  (-2)
#define Err_Redis_Expire                 (-3)
#define Err_Redis_Expire_Or_Not_Exist    (-4)
#define Err_Vsersion_Not_Support         (-5)
#define Err_Redis_Setex                  (-6)
#define Err_Redis_Hmset                  (-7)

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
    m_RedisDBIdx = new RedisDBIdx(&m_xRedisClient);
}

void SRTStorageRedis::Unin()
{
    m_RedisGroup = nullptr;
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

// for read
void SRTStorageRedis::OnWakeupEvent(const void*pData, int nSize)
{
    if (m_QueuePostMsg.size()==0) return;
    pms::StorageMsg store = m_QueuePostMsg.front();
    if (store.mflag()==pms::EMsgFlag::FGROUP)
    {
        std::string str("");
        char key[512] = {'\0'};
        sprintf(key, "grp:%s:%lld", store.storeid().c_str(), store.sequence());
        // grp:storeid:mtype:push:sequence
        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
        if (m_xRedisClient.exists(*m_RedisDBIdx, key))
        {
            if (store.version().compare("1.0.1")==0) {
                LI("group read store.version is:%s\n", store.version().c_str());
                VDATA vdata;
                vdata.push_back("ispush");
                vdata.push_back("mtype");
                vdata.push_back("cont");

                ArrayReply reply;
                if (m_xRedisClient.hmget(*m_RedisDBIdx, key, vdata, reply))
                {
                    store.set_result(Err_Redis_Ok);
                    *store.mutable_ispush() = reply.at(0).str;
                    *store.mutable_mtype() = reply.at(1).str;
                    *store.mutable_content() = reply.at(2).str;
                } else {
                    char* err = m_RedisDBIdx->GetErrInfo();
                    if (err) {
                        LE("m_xRedisClient.hmset err:%s\n", err);
                    }
                    LI("SRTStorageRedis::OnTickEvent g write group msg error\n");
                    store.set_result(Err_Redis_Expire_Or_Not_Exist);
                    //assert(false);
                }
                for (ArrayReply::iterator it=reply.begin();it!=reply.end();++it) {
                     LI("hmget reply it.type:%u, it.str:%s\n", it->type, it->str.c_str());
                }

            } else if (store.version().compare("")==0) {
                LI("group read store.version is:nullllllllllllll\n");
                m_xRedisClient.get(*m_RedisDBIdx, key, str);
                if (str.length()==0) {
                    store.set_result(Err_Redis_Expire_Or_Not_Exist);
                } else {
                    store.set_result(Err_Redis_Ok);
                }
                *store.mutable_content() = str;
            } else {
                LI("group read store.version is:not support\n");
                store.set_result(Err_Vsersion_Not_Support);
            }

        } else {
            store.set_result(Err_Redis_Not_Exist);// key is not exists
            *store.mutable_content() = str;
        }
        LI("SRTStorageRedis::OnWakeupEvent g read key:%s, msgid:%s, storeid:%s, seqn:%lld, maxseqn:%lld, result:%d\n"\
                , key\
                , store.msgid().c_str()\
                , store.storeid().c_str()\
                , store.sequence()\
                , store.maxseqn()\
                , store.result());

    } else if (store.mflag()==pms::EMsgFlag::FSINGLE)
    {
        std::string str("");
        char key[512] = {'\0'};
        sprintf(key, "sgl:%s:%lld", store.storeid().c_str(), store.sequence());
        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
        if (m_xRedisClient.exists(*m_RedisDBIdx, key))
        {
            if (store.version().compare("1.0.1")==0) {
                LI("single read store.version is:%s\n", store.version().c_str());
                VDATA vdata;
                vdata.push_back("ispush");
                vdata.push_back("mtype");
                vdata.push_back("cont");

                ArrayReply reply;
                if (m_xRedisClient.hmget(*m_RedisDBIdx, key, vdata, reply))
                {
                    store.set_result(Err_Redis_Ok);
                    *store.mutable_ispush() = reply.at(0).str;
                    *store.mutable_mtype() = reply.at(1).str;
                    *store.mutable_content() = reply.at(2).str;
                } else {
                    char* err = m_RedisDBIdx->GetErrInfo();
                    if (err) {
                        LE("m_xRedisClient.hmset err:%s\n", err);
                    }
                    LI("SRTStorageRedis::OnTickEvent s write single msg error\n");
                    store.set_result(Err_Redis_Expire_Or_Not_Exist);
                    //assert(false);
                }
                for (ArrayReply::iterator it=reply.begin();it!=reply.end();++it) {
                     LI("hmget reply it.type:%u, it.str:%s\n", it->type, it->str.c_str());
                }
            } else if (store.version().compare("")==0) {
                LI("single read store.version is:null\n");
                m_xRedisClient.get(*m_RedisDBIdx, key, str);
                if (str.length()==0) {
                    store.set_result(Err_Redis_Expire_Or_Not_Exist);
                } else {
                    store.set_result(Err_Redis_Ok);
                }
                *store.mutable_content() = str;
            } else {
                LI("single read store.version is not supporttttt\n");
                store.set_result(Err_Vsersion_Not_Support);
            }

        } else {
            store.set_result(Err_Redis_Not_Exist);// key is not exists
            *store.mutable_content() = str;
        }
        LI("SRTStorageRedis::OnWakeupEvent s read key:%s, msgid:%s, storeid:%s, seqn:%lld, maxseqn:%lld, result:%d\n"\
                , key\
                , store.msgid().c_str()\
                , store.storeid().c_str()\
                , store.sequence()\
                , store.maxseqn()\
                , store.result());
    } else {
        LI("SRTStorageRedis::OnWakeupEvent mflag:%d error\n", store.mflag());
        assert(false);
    }
    if (m_RedisGroup)
    {
        m_RedisGroup->PostData(store.SerializeAsString());
        {
            OSMutexLocker locker(&m_MutexRecvPush);
            m_QueuePostMsg.pop();
        }
    } else {
        assert(false);
    }
    if (m_QueuePostMsg.size()>0)
    {
        this->Signal(Task::kWakeupEvent);
    }
}

// for write
void SRTStorageRedis::OnTickEvent(const void*pData, int nSize)
{
    LI("SRTStorageRedis::OnTickEvent for write...\n");
    if (m_QueuePushMsg.size()==0) return;
    pms::StorageMsg store = m_QueuePushMsg.front();
    if (store.mflag()==pms::EMsgFlag::FGROUP)
    {
        char key[512] = {'\0'};
        sprintf(key, "grp:%s:%lld", store.storeid().c_str(), store.sequence());
        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

        LI("SRTStorageRedis::OnTickEvent g result:%d, storeid:%s, msgid:%s, key:%s, content.len:%d\n"\
                , store.result()\
                , store.storeid().c_str()\
                , store.msgid().c_str()\
                , key\
                , store.content().length());

        if (store.version().c_str()) {
             LI("version is:%s, store.version().length:%d\n", store.version().c_str(), store.version().length());
        } else {
            LI("version is null\n");
        }
        {
            if (store.version().compare("1.0.1")==0) {
                LI("group write store.version is:%s\n", store.version().c_str());
                VDATA vdata;
                vdata.push_back("ispush");
                vdata.push_back(store.ispush());
                vdata.push_back("mtype");
                vdata.push_back(store.mtype());
                vdata.push_back("cont");
                vdata.push_back(store.content());

                if (m_xRedisClient.hmset(*m_RedisDBIdx, key, vdata))
                {
                    store.set_result(Err_Redis_Ok);
                    m_xRedisClient.expire(*m_RedisDBIdx, key, MAX_MESSAGE_EXPIRE_TIME);
                } else {
                    char* err = m_RedisDBIdx->GetErrInfo();
                    if (err) {
                        LE("m_xRedisClient.hmset err:%s\n", err);
                    }
                    LE("SRTStorageRedis::OnTickEvent g write group msg error\n");
                    store.set_result(Err_Redis_Hmset);
                    //assert(false);
                }
            } else if (store.version().compare("")==0) { // else if store.version.compare
                LI("group write store.version is: nulllllllll\n");
                if (m_xRedisClient.setex(*m_RedisDBIdx, key, MAX_MESSAGE_EXPIRE_TIME, store.content().c_str()))
                {
                    store.set_result(Err_Redis_Ok);
                } else {
                    char* err = m_RedisDBIdx->GetErrInfo();
                    if (err) {
                        LE("m_xRedisClient.setex err:%s\n", err);
                    }
                    LE("SRTStorageRedis::OnTickEvent g write group msg error\n");
                    store.set_result(Err_Redis_Setex);
                    //assert(false);
                }
            } else {
                LI("group write store.version not suppoort\n");
                store.set_result(Err_Vsersion_Not_Support);
            }
        }
        LI("SRTStorageRedis::OnTickEvent g write key:%s, msgid:%s, storeid:%s, seqn:%lld, maxseqn:%lld, result:%d\n"\
                , key\
                , store.msgid().c_str()\
                , store.storeid().c_str()\
                , store.sequence()\
                , store.maxseqn()\
                , store.result());
    } else if (store.mflag()==pms::EMsgFlag::FSINGLE)
    {
        char key[512] = {'\0'};
        sprintf(key, "sgl:%s:%lld", store.storeid().c_str(), store.sequence());
        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

        LI("SRTStorageRedis::OnTickEvent s result:%d, storeid:%s, msgid:%s\n"\
                , store.result()\
                , store.storeid().c_str()\
                , store.msgid().c_str());
        {

            if (store.version().compare("1.0.1")==0) {
                LI("single write store.version is:%s\n", store.version().c_str());
                VDATA vdata;
                vdata.push_back("ispush");
                vdata.push_back(store.ispush());
                vdata.push_back("mtype");
                vdata.push_back(store.mtype());
                vdata.push_back("cont");
                vdata.push_back(store.content());

                if (m_xRedisClient.hmset(*m_RedisDBIdx, key, vdata))
                {
                    store.set_result(Err_Redis_Ok);
                    m_xRedisClient.expire(*m_RedisDBIdx, key, MAX_MESSAGE_EXPIRE_TIME);
                } else {
                    char* err = m_RedisDBIdx->GetErrInfo();
                    if (err) {
                        LE("m_xRedisClient.hmset err:%s\n", err);
                    }
                    LE("SRTStorageRedis::OnTickEvent s write group msg error\n");
                    store.set_result(Err_Redis_Hmset);
                    //assert(false);
                }
            } else if (store.version().compare("")==0) {
                LI("single write store.version nullllllll\n");
                if (m_xRedisClient.setex(*m_RedisDBIdx, key, MAX_MESSAGE_EXPIRE_TIME, store.content().c_str()))
                {
                    store.set_result(Err_Redis_Ok);
                } else {
                    char* err = m_RedisDBIdx->GetErrInfo();
                    if (err) {
                        LE("m_xRedisClient.setex err:%s\n", err);
                    }
                    LE("SRTStorageRedis::OnTickEvent s write group msg error\n");
                    store.set_result(Err_Redis_Setex);
                    //assert(false);
                }
            } else {
                LI("single write store.version not suppoort\n");
                store.set_result(Err_Vsersion_Not_Support);
            }
        }
        LI("SRTStorageRedis::OnTickEvent s write key:%s, msgid:%s, storeid:%s, seqn:%lld, maxseqn:%lld, result:%d\n"\
                , key\
                , store.msgid().c_str()\
                , store.storeid().c_str()\
                , store.sequence()\
                , store.maxseqn()\
                , store.result());
    } else {
        LI("SRTStorageRedis::OnTickEvent mflag:%d error\n", store.mflag());
        assert(false);
    }

    if (m_RedisGroup)
    {
        m_RedisGroup->PushData(store.SerializeAsString());
        {
            OSMutexLocker locker(&m_MutexRecvPush);
            m_QueuePushMsg.pop();
        }
    } else {
         assert(false);
    }
    if (m_QueuePushMsg.size()>0)
    {
        this->Signal(Task::kIdleEvent);
    }
}

// for write
void SRTStorageRedis::PushToQueue(pms::StorageMsg request)
{
    {
        OSMutexLocker locker(&m_MutexRecvPush);
        m_QueuePushMsg.push(request);
    }
    this->Signal(Task::kIdleEvent);
}

// for read
void SRTStorageRedis::PostToQueue(pms::StorageMsg request)
{
    {
        OSMutexLocker locker(&m_MutexRecvPush);
        m_QueuePostMsg.push(request);
    }
    this->Signal(Task::kWakeupEvent);
}

void SRTStorageRedis::ConnectionDisconnected()
{
     LI("%s was called\n", __FUNCTION__);
}

///////////////////////////////////////////////////////
