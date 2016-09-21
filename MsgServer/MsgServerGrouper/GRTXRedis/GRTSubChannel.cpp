//
//  GRTSubChannel.cpp
//  dyncRTGrouper
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "GRTSubChannel.h"
#include "RTUtils.hpp"
#include "GRTConnManager.h"
#include "GRTGrouperManager.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#if 1
#include <sys/syscall.h>
#define gettidv1() syscall(__NR_gettid)
#endif

#define TIMEOUT_TS (60*1000)

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


GRTSubChannel::GRTSubChannel(const std::string& ip, int port, const std::string& channel)
: RCPthread(this)
, m_addr(ip)
, m_port(port)
, m_channel(channel)
, m_RedisNodeNum(3)
, m_isRun(false)
{

}

GRTSubChannel::~GRTSubChannel()
{

}

bool GRTSubChannel::Init()
{
    m_RedisList = new RedisNode[m_RedisNodeNum];
    for (int i=0;i<m_RedisNodeNum;++i) {
        m_RedisList[i].dbindex = i;
        m_RedisList[i].host = m_addr.c_str();
        m_RedisList[i].port = m_port;
        m_RedisList[i].passwd = "";
        m_RedisList[i].poolsize = 8;
        m_RedisList[i].timeout = 5;
        m_RedisList[i].role = 0;
    }

    m_xRedisClient.Init(CACHE_TYPE_MAX);
    m_xRedisClient.ConnectRedisCache(m_RedisList, m_RedisNodeNum, CACHE_TYPE_1);
    m_RedisDBIdx = new RedisDBIdx(&m_xRedisClient);
    if (!m_RedisDBIdx)
    {
        LE("GRTSubChannel::Init new RedisDBIdx failed\n");
        if (m_RedisList)
        {
            delete m_RedisList;
            m_RedisList = nullptr;
        }
        return false;
    }
    m_RedisDBIdx->CreateDBIndex(m_channel.c_str(), APHash, CACHE_TYPE_1);

    this->_Init();// this will lunch thread
    return true;
}

bool GRTSubChannel::Unit()
{
    this->_Unin();
    if (m_RedisList)
    {
        delete[] m_RedisList;
        m_RedisList = nullptr;
    }

    m_xRedisClient.FreexRedisConnection(&m_RedisContext);
    if (m_RedisDBIdx)
    {
         delete m_RedisDBIdx;
         m_RedisDBIdx = nullptr;
    }

    m_xRedisClient.Release();
    return true;
}

void GRTSubChannel::Stop()
{
    m_isRun = false;
    std::string key("byebye");
    std::string val;
    if (m_xRedisClient.echo(*m_RedisDBIdx, key, val)) {
        LI("GRTSubChannel::echo channel:%s key:%s, val:%s, success\n", m_channel.c_str(), key.c_str(), val.c_str());
    } else {
        LE("GRTSubChannel::echo channel:%s key:%s, failed\n", m_channel.c_str(), key.c_str());
        return;
    }
}

// from RCPthread

void GRTSubChannel::Run(void* data)
{
    if (!data) return;
    GRTSubChannel* pch = (GRTSubChannel*)data;
    if (!pch) return;
    pch->Subscribe();
    LI("GRTSubChannel::Run was called...pid:%d, tid:%d, pthread_self:%d\n", getpid(), gettidv1(), pthread_self());
    m_isRun = true;
    ReplyData vReply;
    while(m_isRun) {
        if (vReply.size()!=0) {
            vReply.clear();
        }
        if (0==m_xRedisClient.GetReply(&m_RedisContext, vReply)) {
            LI("GRTSubChannel::Run GetReply ok...vReply.size:%lu, m_channel:%s\n", vReply.size(), m_channel.c_str());

            if (vReply.size()!=3) continue;
            if (vReply[0].str.compare("message")!=0) continue;
            if (vReply[1].str.compare(m_channel)!=0) continue;
            if (vReply[2].str.length()==0) continue;

            LI("GRTSubChannel::Run GetReply reply cmd:%s, chl:%s, msg:%s...\n", vReply[0].str.c_str(), vReply[1].str.c_str(), vReply[2].str.c_str());
            std::string grpid("");
            std::string usrid("");
            rapidjson::Document jsonMsgDoc;
            if (!jsonMsgDoc.ParseInsitu<0>((char*)vReply[2].str.c_str()).HasParseError())
            {
                if (jsonMsgDoc.HasMember("groupid"))
                {
                    grpid = jsonMsgDoc["groupid"].GetString();
                    LI("Channel groupid:%s\n", grpid.c_str());
                } else {
                    LE("in jsonMsgDoc not has member groupid, jstr:%s\n", vReply[2].str.c_str());
                    continue;
                }
                if (jsonMsgDoc.HasMember("userid"))
                {
                    usrid = jsonMsgDoc["userid"].GetString();
                    LI("Channel usrid:%s\n", usrid.c_str());
                } else {
                    LE("in jsonMsgDoc not has member userid, jstr:%s\n", vReply[2].str.c_str());
                    continue;
                }
            } else {
                LE("parse msg:%s error\n", vReply[2].str.c_str());
                continue;
            }
            //usreid, flag(add/del)
            if (m_channel.compare("follow_group")==0)
            {
                GRTGrouperManager::GrpMembersStatusMap users;
                users[usrid] = GRTGrouperManager::GrpMemStatus::ADD;
                GRTGrouperManager::Instance().UpdateGroupMembers(grpid, users);
            } else if (m_channel.compare("unfollow_group")==0)
            {
                GRTGrouperManager::GrpMembersStatusMap users;
                users[usrid] = GRTGrouperManager::GrpMemStatus::DEL;
                GRTGrouperManager::Instance().UpdateGroupMembers(grpid, users);
            }
        }
    }
    LI("GRTSubChannel::Run while over...\n");
    pch->Unsubscribe();
    LI("GRTSubChannel::Run finished over...\n");
}


////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

void GRTSubChannel::Subscribe()
{
    KEYS key;
    key.push_back(m_channel);
    if (m_xRedisClient.subscribe(*m_RedisDBIdx, key, m_RedisContext)) {
        LI("GRTSubChannel::Subscribe channel:%s success\n", m_channel.c_str());
    } else {
        LE("GRTSubChannel::Subscribe channel:%s failed\n", m_channel.c_str());
        return;
    }
}

void GRTSubChannel::Unsubscribe()
{
    KEYS key;
    key.push_back(m_channel);
    if (m_xRedisClient.unsubscribe(*m_RedisDBIdx, key, m_RedisContext)) {
        LI("GRTSubChannel::Unsubscribe channel:%s success\n", m_channel.c_str());
    } else {
        LE("GRTSubChannel::Unsubscribe failed\n");
        return;
    }
}
