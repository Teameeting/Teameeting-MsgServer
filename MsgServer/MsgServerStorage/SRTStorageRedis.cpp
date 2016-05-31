//
//  SRTStorageRedis.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTStorageRedis.h"
#include "SRTRedisManager.h"
#include "../MsgServer/proto/storage_msg.pb.h"

void SRTStorageRedis::Init(SRTRedisManager* manager, const std::string& ip, int port)
{
    //LI("SRTStorageRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);
    this->SetHostAddr(ip.c_str(), port);
    this->Connect();
    m_RedisManager = manager;
    RequestResponse.connect(m_RedisManager, &SRTRedisManager::OnRequestSeqn);
}

void SRTStorageRedis::Unin()
{
    //LI("SRTStorageRedis::Unin was called, host:%s\n", GetHostForTest().c_str());
    RequestResponse.disconnect(m_RedisManager);
    this->DisConn();
}

// from RTEventLooper
void SRTStorageRedis::OnPostEvent(const char*pData, int nSize)
{

}

void SRTStorageRedis::OnWakeupEvent(const char*pData, int nSize)
{

}

void SRTStorageRedis::OnPushEvent(const char*pData, int nSize)
{
    if (!pData || nSize<=0) return;
    long long seq = 0;
    std::string str(pData, nSize);
    //pms::StorageMsg request;
    //request.ParseFromString(str);
    //GenericIncrId(request.userid(), &seq);
    //RequestResponse(request.userid(), request.msgid(), seq);
}

bool SRTStorageRedis::IsTheSameRedis(const std::string& host, int port)
{
    return ((this->GetHost().compare(host)==0) && (this->GetPort()==port));
}
///////////////////////////////////////////////////////
