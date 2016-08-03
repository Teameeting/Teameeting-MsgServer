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
: RTEventLooper()
, m_addr(ip)
, m_port(port)
, m_channel(channel)
, m_RedisContext(NULL)
, m_RedisConn(NULL)
{
    AddObserver(this);
}

GRTSubChannel::~GRTSubChannel()
{
    DelObserver(this);
    Unit();
}

bool GRTSubChannel::Init()
{
    m_RedisList = new RedisNode;
    if (!m_RedisList)
    {
        LE("GRTSubChannel::Init new RedisNode failed\n");
        return false;
    }
    m_RedisList->dbindex = 0;
    m_RedisList->host = m_addr.c_str();
    m_RedisList->port = m_port;
    m_RedisList->passwd = "";
    m_RedisList->poolsize = 8;
    m_RedisList->timeout = 5;
    m_RedisList->role = 0;

    m_xRedisClient.Init(CACHE_TYPE_MAX);
    m_xRedisClient.ConnectRedisCache(m_RedisList, 1, CACHE_TYPE_1);
    m_RedisDBIdx = new RedisDBIdx(&m_xRedisClient);
    if (!m_RedisDBIdx)
    {
        LE("GRTSubChannel::Init new RedisDBIdx failed\n");
        if (m_RedisList)
        {
            delete m_RedisList;
            m_RedisList = NULL;
        }
        return false;
    }
    m_RedisDBIdx->CreateDBIndex(m_channel.c_str(), APHash, CACHE_TYPE_1);
    m_RedisConn = m_xRedisClient.GetRedisConn(*m_RedisDBIdx);
    m_RedisContext = m_xRedisClient.GetxRedisContext(m_RedisConn);
    return true;
}

bool GRTSubChannel::Unit()
{
    m_xRedisClient.FreexRedisConn(m_RedisConn);
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
    return true;
}

void GRTSubChannel::Subscribe()
{
    if (m_xRedisClient.subscribe(*m_RedisDBIdx, m_RedisConn, m_channel)) {
        LI("GRTSubChannel::Subscribe channel:%s success\n", m_channel.c_str());
        m_xRedisClient.FreeReply((redisReply*)m_RedisContext->reader->reply);
        m_RedisContext->reader->reply = NULL;
        this->Signal(kIdleEvent);
    } else {
        LE("GRTSubChannel::Subscribe channel:%s failed\n", m_channel.c_str());
        std::string err("subscribe");
        this->PostData(err);
        return;
    }
}

void GRTSubChannel::Unsubscribe()
{
    if (m_xRedisClient.unsubscribe(*m_RedisDBIdx, m_RedisConn, m_channel)) {
        LI("GRTSubChannel::Unsubscribe channel:%s success\n", m_channel.c_str());
        m_xRedisClient.FreeReply((redisReply*)m_RedisContext->reader->reply);
        m_RedisContext->reader->reply = NULL;
    } else {
        LE("GRTSubChannel::Subscribe failed\n");
        std::string err("unsubscribe");
        this->PostData(err);
        return;
    }
}


// from RTEventLooper
void GRTSubChannel::OnPostEvent(const char*pData, int nSize)
{
    if (!pData || nSize==0) return;
    if (strcmp(pData, "subscribe")==0) {
        LI("GRTSubChannel::OnPostEvent recv subscribe error\n");
        Subscribe();
    } else if (strcmp(pData, "unsubscribe")==0) {
        LI("GRTSubChannel::OnPostEvent recv unsubscribe error\n");
        Unsubscribe();
    }
}


void GRTSubChannel::OnWakeupEvent(const void*pData, int nSize)
{

}

void GRTSubChannel::OnPushEvent(const char*pData, int nSize)
{

}

void GRTSubChannel::OnTickEvent(const void*pData, int nSize)
{
    LI("GRTSubChannel::OnTickEvent was called...\n");
    redisReply *reply = NULL;

    if (0==m_xRedisClient.GetReply(m_RedisContext, &reply)) {
        LI("GRTSubChannel::OnTickEvent GetReply...\n");
        if (reply->type==REDIS_REPLY_ARRAY) {
            assert(reply->elements==3);
            std::string cmd(reply->element[0]->str, reply->element[0]->len);
            std::string chl(reply->element[1]->str, reply->element[1]->len);
            std::string msg(reply->element[2]->str, reply->element[2]->len);
            Assert(m_channel.compare(chl)==0);
            if (cmd.compare("message")==0) {
                LI("you recv publish msg from channel:%s, msg:%s\n", chl.c_str(), msg.c_str());
                //TODO:
                //parse the msg received
                std::string grpid("");
                std::string usrid("");
                rapidjson::Document jsonMsgDoc;
                if (!jsonMsgDoc.ParseInsitu<0>((char*)msg.c_str()).HasParseError())
                {
                    if (jsonMsgDoc.HasMember("groupid"))
                    {
                        grpid = jsonMsgDoc["groupid"].GetString();
                        LI("Channel groupid:%s\n", grpid.c_str());
                    } else {
                        LE("in jsonMsgDoc not has member groupid\n");
                    }
                    if (jsonMsgDoc.HasMember("userid"))
                    {
                        usrid = jsonMsgDoc["userid"].GetString();
                        LI("Channel usrid:%s\n", usrid.c_str());
                    } else {
                        LE("in jsonMsgDoc not has member userid\n");
                    }

                } else {
                     LE("parse msg:%s error\n", msg.c_str());
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
            } else {
                LI("pub/sub cmd not handled:%s\n", cmd.c_str());
            }
        } else {
            LE("GRTSubChannel::OnTickEvent reply->type:%d not handled\n", reply->type);
        }
        m_xRedisClient.FreeReply(reply);
        reply = NULL;
    }
    this->Signal(kIdleEvent);
}

void GRTSubChannel::ConnectionDisconnected()
{
     LI("%s was called\n", __FUNCTION__);
}


////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

