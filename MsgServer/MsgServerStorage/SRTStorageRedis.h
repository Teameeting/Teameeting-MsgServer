//
//  SRTStorageRedis.h
//  MsgServerStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTStorageRedis__
#define __MsgServerStorage__SRTStorageRedis__

#include "RTEventLooper.h"
#include "MsgServer/proto/storage_msg.pb.h"
#include "OSMutex.h"
#include <hiredis/hiredis.h>
#include "xRedisClient.h"

class SRTRedisGroup;

class SRTStorageRedis : public RTEventLooper{

public:

    void Init(SRTRedisGroup* group, const std::string& ip, int port);
    void Unin();

    virtual void MakeAbstract() {}
    bool IsTheSameRedis(const std::string& host, int port);

// from RTEventLooper
public:
    virtual void OnPostEvent(const char*pData, int nSize);
    virtual void OnSendEvent(const void*pData, int nSize) {}
    virtual void OnWakeupEvent(const void*pData, int nSize);
    virtual void OnPushEvent(const char*pData, int nSize);
    virtual void OnTickEvent(const void*pData, int nSize);
private:
    std::string                      m_Ip;
    int                              m_Port;
    SRTRedisGroup*                   m_RedisGroup;
    pms::StorageMsg                  m_RecvStoreMsg;

    xRedisClient                     m_xRedisClient;
    RedisDBIdx*                      m_redisDBIdx;
    RedisNode*                       m_RedisList;

};


#endif /* defined(__MsgServerStorage__SRTStorageRedis__) */
