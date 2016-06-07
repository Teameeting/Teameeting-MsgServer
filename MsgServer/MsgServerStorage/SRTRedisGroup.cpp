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

#define REDIS_GROUP_CLIENT (10)
#define REDIS_GROUP_CLIENT_MAX (REDIS_GROUP_CLIENT*10007)
#define PACKED_MSG_ONCE_NUM (10)

static int g_push_request_counter = 0;
static int g_post_event_counter = 0;
static int g_push_redis_index = 0;


SRTRedisGroup::SRTRedisGroup(SRTTransferSession *sess, const std::string& ip, int port)
: m_Port(port)
, m_Ip(ip)
{
    for (int i=0;i<REDIS_GROUP_CLIENT;++i)
    {
        SRTStorageRedis* redis = new SRTStorageRedis();;
        redis->Init(this, ip, port);
        m_Redises.push_back(redis);
    }
    m_ResponseSender.Init(sess);
}

SRTRedisGroup::~SRTRedisGroup()
{
    m_ResponseSender.Unin();
    for (auto x : m_Redises)
    {
        x->Unin();
        delete x;
        x = nullptr;
    }
    m_Redises.clear();
}

void SRTRedisGroup::DispatchPushData(const char*pData, int nSize)
{
    if (!pData || nSize <=0)
    {
        printf("SRTRedisGroup::DispatchPushData pData or size is wrong\n", nSize);
        return;
    }
    //printf("SRTRedisGroup::DispatchPushData g_push_request_counter:%d\n", ++g_push_request_counter);
    std::string str(pData, nSize);
    m_RecvPackedMsg.ParseFromString(str);
    {
        for (int i=0;i<m_RecvPackedMsg.msgs_size();++i)
        {
            m_Redises[g_push_redis_index++%REDIS_GROUP_CLIENT]->PushData(m_RecvPackedMsg.msgs(i).SerializeAsString());
            m_RecvPackedMsg.mutable_msgs(i)->Clear();
            if (g_push_redis_index == REDIS_GROUP_CLIENT_MAX)
                g_push_redis_index = 0;
        }
    }
}


void SRTRedisGroup::OnPostEvent(const char*pData, int nSize)
{
    m_ResponseSender.PushResponseData(pData, nSize);
}

void SRTRedisGroup::OnSendEvent(const void*pData, int nSize)
{

}

void SRTRedisGroup::OnWakeupEvent(const void*pData, int nSize)
{

}

void SRTRedisGroup::OnPushEvent(const char*pData, int nSize)
{

}

void SRTRedisGroup::OnTickEvent(const void*pData, int nSize)
{

}
///////////////////////////////////////////////////////
