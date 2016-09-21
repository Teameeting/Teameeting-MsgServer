//
//  CRTXRedis.cpp
//  dyncRTConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTXRedis.h"
#include "rtklog.h"

#define MAX_MESSAGE_EXPIRE_TIME (7*24*60*60)

#define Err_Redis_Ok                     (0)
#define Err_Redis_Not_Exist              (-1)
#define Err_Redis_Exist                  (-2)
#define Err_Redis_Expire                 (-3)
#define Err_Redis_Expire_Or_Not_Exist    (-4)
#define Err_Vsersion_Not_Support         (-5)
#define Err_Redis_Setex                  (-6)
#define Err_Redis_Hmset                  (-7)
#define Err_Redis_Type                   (-8)

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

void CRTXRedis::Init(const std::string& ip, int port)
{
    //LI("CRTXRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);
    m_Ip = ip;
    m_Port = port;
    m_RedisNodeNum = 1;

    m_RedisList = new RedisNode[m_RedisNodeNum];
    for (int i=0;i<m_RedisNodeNum;++i) {
        m_RedisList[i].dbindex = i;
        m_RedisList[i].host = m_Ip.c_str();
        m_RedisList[i].port = m_Port;
        m_RedisList[i].passwd = "";
        m_RedisList[i].poolsize = 8;
        m_RedisList[i].timeout = 5;
        m_RedisList[i].role = 0;
    }

    m_xRedisClient.Init(CACHE_TYPE_MAX);
    m_xRedisClient.ConnectRedisCache(m_RedisList, 1, CACHE_TYPE_1);
    m_RedisDBIdx = new RedisDBIdx(&m_xRedisClient);
    if (!m_RedisDBIdx)
    {
        LE("GRTSubChannel::Init new RedisDBIdx failed\n");
        if (m_RedisList)
        {
            delete m_RedisList;
            m_RedisList = nullptr;
        }
    }
}

void CRTXRedis::Unin()
{
    if (m_RedisList)
    {
        delete[] m_RedisList;
        m_RedisList = nullptr;
    }
    if (m_RedisDBIdx)
    {
         delete m_RedisDBIdx;
         m_RedisDBIdx = nullptr;
    }
    m_xRedisClient.Release();
}

bool CRTXRedis::Echo()
{
     RedisDBIdx dbi(&m_xRedisClient);
     dbi.CreateDBIndex("echo", APHash, CACHE_TYPE_1);
     std::string val;
     m_xRedisClient.echo(dbi, "echo", val);
     if (val.compare("echo")==0)
     {
        return true;
     } else {
        return false;
     }
}

// redis user push setting key:'push:set:module:userid' by hmset, 'token':token, 'push':enablepush
bool CRTXRedis::SetSettingPush(const std::string& userid, int module, const std::string& field, const std::string& value)
{
    VDATA vdata;
    vdata.push_back(field);
    vdata.push_back(value);

    char key[128] = {0};
    sprintf(key, "push:set:%d:%s", module, userid.c_str());

    if (!m_xRedisClient.hmset(*m_RedisDBIdx, key, vdata))
    {
        char* err = m_RedisDBIdx->GetErrInfo();
        if (err) {
            LE("CRTXRedis::SetSettingPush m_xRedisClient.hmset err:%s\n", err);
        }
        return false;
    }
    return true;
}

bool CRTXRedis::GetSettingPush(const std::string& userid, int module, const std::string& field, std::string& value)
{
    ArrayReply reply;
    VDATA vdata;
    vdata.push_back(field);

    char key[128] = {0};
    sprintf(key, "push:set:%d:%s", module, userid.c_str());
    printf("CRTXRedis::GetSettingPush key is:%s\n\n", key);

    if (!m_xRedisClient.hmget(*m_RedisDBIdx, key, vdata, reply))
    {
        char* err = m_RedisDBIdx->GetErrInfo();
        if (err) {
            LE("CRTXRedis::StoreSettingPush m_xRedisClient.hmset err:%s\n", err);
        }
        return false;
    }
    value = reply.at(0).str;
    return true;
}

bool CRTXRedis::GetSettingPush(const std::string& userid, int module, const std::string& field1, std::string& value1, const std::string& field2, std::string& value2)
{
    ArrayReply reply;
    VDATA vdata;
    vdata.push_back(field1);
    vdata.push_back(field2);

    char key[128] = {0};
    sprintf(key, "push:set:%d:%s", module, userid.c_str());
    printf("CRTXRedis::GetSettingPush key is:%s\n\n", key);

    if (!m_xRedisClient.hmget(*m_RedisDBIdx, key, vdata, reply))
    {
        char* err = m_RedisDBIdx->GetErrInfo();
        if (err) {
            LE("CRTXRedis::StoreSettingPush m_xRedisClient.hmset err:%s\n", err);
        }
        return false;
    }
    value1 = reply.at(0).str;
    value2 = reply.at(1).str;
    return true;
}

///////////////////////////////////////////////////////
