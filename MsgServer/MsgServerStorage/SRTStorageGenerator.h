//
//  SRTStorageGenerator.h
//  MsgServerStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTStorageGenerator__
#define __MsgServerStorage__SRTStorageGenerator__

#include <stdio.h>
#include <iostream>
#include <vector>
#include "RTEventLooper.h"
#include "SRTRedisManager.h"

#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

#define REQUEST_DATA_SIZE (int(sizeof(SRTStorageGenerator::RequestData)))

class SRTTransferSession;

class SRTStorageGenerator : public RTEventLooper {
public:
    SRTStorageGenerator();
    virtual ~SRTStorageGenerator();

    void Init(SRTTransferSession* sess);
    void Unin();

public:


// from RTEventLooper
public:
    virtual void OnPostEvent(const char*pData, int nSize);
    virtual void OnSendEvent(const void*pData, int nSize) {}
    virtual void OnWakeupEvent(const char*pData, int nSize);
    virtual void OnPushEvent(const char*pData, int nSize);
    virtual void OnTickEvent(const void*pData, int nSize) {}

private:
    long long mSendResponse;
    SRTTransferSession*                 m_Session;
    std::vector<std::string>            m_RedisHosts;
    SInt64                              m_LastUpdateTime;
    SRTRedisManager                     m_RedisManager;
};

#endif /* defined(__MsgServerStorage__SRTStorageGenerator__) */
