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

#include <hiredis/hiredis.h>
#include "xRedisClient.h"
#include "RCPthread.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class GRTSubChannel
    : public RCPthread {
public:
    GRTSubChannel(const std::string& ip, int port, const std::string& channel);
    virtual ~GRTSubChannel();

    bool Init();
    bool Unit();

    void Stop();

// from RCPthread
public:
    virtual void Run(void* data);

private:
    void Subscribe();
    void Unsubscribe();

private:
    std::string                 m_addr;
    int                         m_port;
    std::string                 m_channel;
    int                         m_RedisNodeNum;

    bool                        m_isRun;

    RedisNode*                  m_RedisList;
    xRedisClient                m_xRedisClient;
    RedisDBIdx*                 m_RedisDBIdx;
    xRedisContext               m_RedisContext;
};

#endif /* defined(__MsgServerGrouper__GRTSubChannel__) */
