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

static int g_push_redisrequest_counter = 0;

SRTRedisManager::SRTRedisManager()
: m_RedisGroup(nullptr)
{
}

SRTRedisManager::~SRTRedisManager()
{
}

void SRTRedisManager::Init(SRTTransferSession* sess)
{
    //TODO:
    // if redis crash, m_RedisNum should be notifyed
    SRTStorageManager::Instance().GetRedisHosts(&m_RedisHosts);
    char ip[16] = {0};
    int port = 0;
    sscanf(m_RedisHosts[0].c_str(), "%s %d", ip, &port);
    printf("Generator Init ip:%s, port:%d\n", ip, port);
    m_RedisGroup = new SRTRedisGroup(sess, ip, port);
}

void SRTRedisManager::Unin()
{
    delete m_RedisGroup;
    m_RedisGroup = nullptr;
}


void SRTRedisManager::PushRedisRequest(const char* request, int len)
{
    m_RedisGroup->DispatchPushData(request, len);
}

///////////////////////////////////////////////////////
