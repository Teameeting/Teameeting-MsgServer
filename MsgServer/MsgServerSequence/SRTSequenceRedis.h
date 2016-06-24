//
//  SRTSequenceRedis.h
//  MsgServerSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerSequence__SRTSequenceRedis__
#define __MsgServerSequence__SRTSequenceRedis__

#include "RTEventLooper.h"
#include "sigslot.h"
#include "ProtoCommon.h"
#include <hiredis/hiredis.h>
#include "xRedisClient.h"

class SRTRedisManager;

class SRTSequenceRedis : public RTEventLooper{

public:

    SRTSequenceRedis(){}
    virtual ~SRTSequenceRedis(){}

    void Init(SRTRedisManager* manager, const std::string& ip, int port);
    void Unin();

    bool IsTheSameRedis(const std::string& host, int port)
    {
        return ((m_Ip.compare(host)==0) && (m_Port==port));
    }

    sigslot::signal2<const pms::StorageMsg&, int64> WriteResponse;
    sigslot::signal2<const pms::StorageMsg&, int64> ReadResponse;

// from RTEventLooper
public:
    virtual void OnPostEvent(const char*pData, int nSize);
    virtual void OnSendEvent(const void*pData, int nSize) {}
    virtual void OnWakeupEvent(const void*pData, int nSize){}
    virtual void OnPushEvent(const char*pData, int nSize);
    virtual void OnTickEvent(const void*pData, int nSize) {}
private:
    std::string                      m_Ip;
    int                              m_Port;
    SRTRedisManager* m_RedisManager;

    xRedisClient                     m_xRedisClient;
    RedisDBIdx*                      m_RedisDBIdx;
    RedisNode*                       m_RedisList;
};


#endif /* defined(__MsgServerSequence__SRTSequenceRedis__) */
