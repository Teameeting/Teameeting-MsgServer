//
//  SRTSequenceGenerator.cpp
//  dyncRTSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTSequenceGenerator.h"
#include "SRTTransferSession.h"
#include "SRTSequenceManager.h"

#define SESSION_TIMEOUT (90*1000)
#define REDIS_GROUPS (1)

SRTSequenceGenerator::SRTSequenceGenerator()
    : m_LastUpdateTime(0)
{
}

SRTSequenceGenerator::~SRTSequenceGenerator()
{
}

void SRTSequenceGenerator::Init(SRTTransferSession* sess)
{
    m_Session = sess;
    m_RedisManager.Init(sess);
}

void SRTSequenceGenerator::Unin()
{
    m_RedisManager.Unin();
    m_Session = nullptr;
}

// from RTEventLooper
void SRTSequenceGenerator::OnPostEvent(const char*pData, int nSize)
{
    if (!pData || nSize <= 0) {
        return;
    }
    std::string str(pData, nSize);
    pms::PackedSeqnMsg packed;
    packed.ParseFromString(str);
    for(int i=0;i<packed.msgs_size();++i)
    {
        if (packed.msgs(i).userid().compare("keepalive")==0)
        {
            //if here is not get in time, the following will be work too
            if (m_Session && m_Session->IsLiveSession())
                m_Session->RefreshTime();
            continue;
        }
        m_RedisManager.PushRedisRequest(packed.msgs(i).SerializeAsString());
    }
    SInt64 curTime = OS::Milliseconds();
    if (m_LastUpdateTime + SESSION_TIMEOUT < curTime)
    {
        m_LastUpdateTime = curTime;
        if (m_Session && m_Session->IsLiveSession())
        {
            m_Session->RefreshTime();
        } else {
            Assert(false);
        }
    }
}

void SRTSequenceGenerator::OnPushEvent(const char*pData, int nSize)
{

}

///////////////////////////////////////////////////////
