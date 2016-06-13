//
//  SRTSequenceRedis.h
//  MsgServerSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerSequence__SRTSequenceRedis__
#define __MsgServerSequence__SRTSequenceRedis__

#include "RTHiredis.h"
#include "RTEventLooper.h"
#include "sigslot.h"

class SRTRedisManager;

class SRTSequenceRedis : public RTHiredis
                       , public RTEventLooper{

public:

    void Init(SRTRedisManager* manager, const std::string& ip, int port);
    void Unin();

    virtual void MakeAbstract() {}
    bool IsTheSameRedis(const std::string& host, int port);
    void SetResult(long long seq) { m_Result = seq; }
    long long GetResult() { return m_Result; }
    bool IsConnected() { return this->CmdPing(); }

    std::string GetHostForTest() { return this->GetHost(); }

    sigslot::signal3<const std::string&, const std::string&, long long> WriteResponse;
    sigslot::signal3<const std::string&, const std::string&, long long> ReadResponse;

// from RTEventLooper
public:
    virtual void OnPostEvent(const char*pData, int nSize);
    virtual void OnSendEvent(const void*pData, int nSize) {}
    virtual void OnWakeupEvent(const void*pData, int nSize){}
    virtual void OnPushEvent(const char*pData, int nSize);
    virtual void OnTickEvent(const void*pData, int nSize) {}
private:
    long long   m_Result;
    SRTRedisManager* m_RedisManager;
};


#endif /* defined(__MsgServerSequence__SRTSequenceRedis__) */
