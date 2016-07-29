//
//  GRTSubChannel.h
//  MsgServerGrouper
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerGrouper__GRTSubChannel__
#define __MsgServerGrouper__GRTSubChannel__

#include <stdio.h>
#include <iostream>
#include <map>
#include "RTEventLooper.h"

#include <hiredis/hiredis.h>
#include "xRedisClient.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class GRTSubChannel
    : public RTEventLooper
    , public RTObserverConnection{
public:
    GRTSubChannel(const std::string& ip, int port, const std::string& channel);
    virtual ~GRTSubChannel();

    bool Init();
    bool Unit();

    void Subscribe();
    void Unsubscribe();

// from RTEventLooper
public:
	virtual void OnPostEvent(const char*pData, int nSize);
	virtual void OnSendEvent(const void*pData, int nSize) {}
	virtual void OnWakeupEvent(const void*pData, int nSize);
	virtual void OnPushEvent(const char*pData, int nSize);
	virtual void OnTickEvent(const void*pData, int nSize);

// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();
private:
    std::string                 m_addr;
    int                         m_port;
    std::string                 m_channel;

    xRedisClient                m_xRedisClient;
    RedisDBIdx*                 m_RedisDBIdx;
    RedisNode*                  m_RedisList;
    xRedisContext*              m_RedisContext;
    RedisConn*                  m_RedisConn;
};

#endif /* defined(__MsgServerGrouper__GRTSubChannel__) */
