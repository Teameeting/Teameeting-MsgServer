//
//  SRTSequenceRedis.cpp
//  dyncRTSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTSequenceRedis.h"
#include "SRTRedisManager.h"
#include "../MsgServer/proto/storage_msg.pb.h"

void SRTSequenceRedis::Init(SRTRedisManager* manager, const std::string& ip, int port)
{
    //LI("SRTSequenceRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);
    this->SetHostAddr(ip.c_str(), port);
    this->Connect();
    m_RedisManager = manager;
    WriteResponse.connect(m_RedisManager, &SRTRedisManager::OnWriteSeqn);
    ReadResponse.connect(m_RedisManager, &SRTRedisManager::OnReadSeqn);
}

void SRTSequenceRedis::Unin()
{
    //LI("SRTSequenceRedis::Unin was called, host:%s\n", GetHostForTest().c_str());
    ReadResponse.disconnect(m_RedisManager);
    WriteResponse.disconnect(m_RedisManager);
    this->DisConn();
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
        printf("SRTSequenceRedis::OnPostEvent msgid:%s\n", request.msgid().c_str());
        long long seq = 0;
        CmdGet(request.userid(), &seq);
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
        printf("SRTSequenceRedis::OnPushEvent msgid:%s\n", request.msgid().c_str());
        long long seq = 0;
        GenericIncrId(request.userid(), &seq);
        printf("OnPushEvent IncrSeq is:%lld\n", seq);
        WriteResponse(request, seq);
    }
}

bool SRTSequenceRedis::IsTheSameRedis(const std::string& host, int port)
{
    return ((this->GetHost().compare(host)==0) && (this->GetPort()==port));
}
///////////////////////////////////////////////////////
