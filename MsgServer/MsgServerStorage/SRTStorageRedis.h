//
//  SRTStorageRedis.h
//  MsgServerStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTStorageRedis__
#define __MsgServerStorage__SRTStorageRedis__

#include <queue>
#include "RTEventLooper.h"
#include "OSMutex.h"
#include <hiredis/hiredis.h>
#include "xRedisClient.h"
#include "RTObserverConnection.h"

#include "ProtoCommon.h"

class SRTRedisGroup;

class SRTStorageRedis : public RTEventLooper, public RTObserverConnection{

public:
    SRTStorageRedis(){ AddObserver(this); }
    virtual ~SRTStorageRedis(){ DelObserver(this); }

    void Init(SRTRedisGroup* group, const std::string& ip, int port);
    void Unin();

    bool IsTheSameRedis(const std::string& host, int port)
    {
        return ((m_Ip.compare(host)==0) && (m_Port==port));
    }

    void PushToQueue(pms::StorageMsg request);
    void PostToQueue(pms::StorageMsg request);

// from RTEventLooper
public:
    virtual void OnPostEvent(const char*pData, int nSize){}
    virtual void OnSendEvent(const void*pData, int nSize) {}
    virtual void OnWakeupEvent(const void*pData, int nSize);
    virtual void OnPushEvent(const char*pData, int nSize){}
    virtual void OnTickEvent(const void*pData, int nSize);

// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();
private:
    std::string                      m_Ip;
    int                              m_Port;
    SRTRedisGroup*                   m_RedisGroup;
    pms::StorageMsg                  m_RecvStoreMsg;

    xRedisClient                     m_xRedisClient;
    RedisDBIdx*                      m_RedisDBIdx;
    RedisNode*                       m_RedisList;

    OSMutex                          m_MutexRecvPush;
    std::queue<pms::StorageMsg>      m_QueuePushMsg;

    OSMutex                          m_MutexRecvPost;
    std::queue<pms::StorageMsg>      m_QueuePostMsg;

};

#endif /* defined(__MsgServerStorage__SRTStorageRedis__) */
