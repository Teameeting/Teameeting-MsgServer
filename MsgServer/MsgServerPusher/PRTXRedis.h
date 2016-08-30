//
//  PRTXRedis.h
//  MsgServerPusher
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerPusher__PRTXRedis__
#define __MsgServerPusher__PRTXRedis__

#include <hiredis/hiredis.h>
#include "xRedisClient.h"
#include "OSMutex.h"

#include "ProtoCommon.h"

class PRTXRedis {

public:
    PRTXRedis(){}
    virtual ~PRTXRedis(){}

    void Init(const std::string& ip, int port);
    void Unin();

    bool Echo();
    bool SetSettingPush(const std::string& userid, int module, const std::string& field, const std::string& value);
    bool GetSettingPush(const std::string& userid, int module, const std::string& field, std::string& value);
    bool SetNeedPushMsg(const std::string& devType, const std::string& value);
    bool GetNeedPushMsg(const std::string& devType, ArrayReply& reply, int64_t start, int64_t stop);
    //bool TrimListMsg(const std::string& devType, int64_t start, int64_t stop);
    bool LenListMsg(const std::string& devType, int64_t count);

private:
    std::string                      m_Ip;
    int                              m_Port;
    int                              m_RedisNodeNum;

    xRedisClient                     m_xRedisClient;
    RedisDBIdx*                      m_RedisDBIdx;
    RedisNode*                       m_RedisList;

    OSMutex                          m_MutexLPush;
};

#endif /* defined(__MsgServerPusher__PRTXRedis__) */
