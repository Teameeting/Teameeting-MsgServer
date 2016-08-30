//
//  SRTRedisGroup.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTRedisGroup.h"
#include "SRTStorageManager.h"
#include "SRTTransferSession.h"
#include "SRTStorageRedis.h"


#define REDIS_GROUP_CLIENT (10)
#define REDIS_GROUP_CLIENT_MAX (REDIS_GROUP_CLIENT*10007)
#define PACKED_MSG_ONCE_NUM (10)

static int g_push_request_counter = 0;
static int g_post_event_counter = 0;

static int g_push_redis_index = 0;
static int g_post_redis_index = 0;


SRTRedisGroup::SRTRedisGroup(SRTTransferSession *sess, const std::string& ip, int port)
: m_Port(port)
, m_Ip(ip)
{
    for (int i=0;i<REDIS_GROUP_CLIENT;++i)
    {
        SRTStorageRedis* redis = new SRTStorageRedis();;
        redis->Init(this, ip, port);
        m_WriteRedises.push_back(redis);
    }

    for (int i=0;i<REDIS_GROUP_CLIENT;++i)
    {
        SRTStorageRedis* redis = new SRTStorageRedis();;
        redis->Init(this, ip, port);
        m_ReadRedises.push_back(redis);
    }
    m_ResponseSender.Init(sess);
    AddObserver(this);
}

SRTRedisGroup::~SRTRedisGroup()
{
    DelObserver(this);
    m_ResponseSender.Unin();
    for (auto x : m_ReadRedises)
    {
        x->Unin();
        delete x;
        x = nullptr;
    }
    m_ReadRedises.clear();
    for (auto x : m_WriteRedises)
    {
        x->Unin();
        delete x;
        x = nullptr;
    }
    m_WriteRedises.clear();
}

void SRTRedisGroup::DispatchPushData(const std::string& data)
{
    //LI("SRTRedisGroup::DispatchPushData g_push_request_counter:%d\n", ++g_push_request_counter);
    if (!m_RecvPushMsg.ParseFromString(data)) return;
    {
        for (int i=0;i<m_RecvPushMsg.msgs_size();++i)
        {
            if (m_RecvPushMsg.msgs(i).ruserid().length()==0)
            {
                LI("%s m_RecvPushMsg.msgs(%d) ruserid length is 0\n", __FUNCTION__, i);
                break;
            }
            m_WriteRedises[g_push_redis_index++%REDIS_GROUP_CLIENT]->PushToQueue(m_RecvPushMsg.msgs(i));
            m_RecvPushMsg.mutable_msgs(i)->Clear();
            if (g_push_redis_index == REDIS_GROUP_CLIENT_MAX)
                g_push_redis_index = 0;
        }
    }
}

void SRTRedisGroup::DispatchPostData(const std::string& data)
{
    //LI("SRTRedisGroup::DispatchPostData g_post_request_counter:%d\n", ++g_post_request_counter);
    if (!m_RecvPostMsg.ParseFromString(data)) return;
    {
        for (int i=0;i<m_RecvPostMsg.msgs_size();++i)
        {
            if (m_RecvPostMsg.msgs(i).ruserid().length()==0)
            {
                LI("%s m_RecvPostMsg.msgs(%d) ruserid length is 0\n", __FUNCTION__, i);
                break;
            }
            m_ReadRedises[g_post_redis_index++%REDIS_GROUP_CLIENT]->PostToQueue(m_RecvPostMsg.msgs(i));
            m_RecvPostMsg.mutable_msgs(i)->Clear();
            if (g_post_redis_index == REDIS_GROUP_CLIENT_MAX)
                g_post_redis_index = 0;
        }
    }
}

void SRTRedisGroup::OnPostEvent(const char*pData, int nSize)
{
    m_ResponseSender.PostResponseData(pData, nSize);
}

void SRTRedisGroup::OnSendEvent(const void*pData, int nSize)
{

}

void SRTRedisGroup::OnWakeupEvent(const void*pData, int nSize)
{

}

void SRTRedisGroup::OnPushEvent(const char*pData, int nSize)
{
    m_ResponseSender.PushResponseData(pData, nSize);
}

void SRTRedisGroup::OnTickEvent(const void*pData, int nSize)
{

}

void SRTRedisGroup::ConnectionDisconnected()
{
     LI("%s was called\n", __FUNCTION__);
}

///////////////////////////////////////////////////////
