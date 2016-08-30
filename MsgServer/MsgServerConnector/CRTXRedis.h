//
//  CRTXRedis.h
//  MsgServerConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTXRedis__
#define __MsgServerConnector__CRTXRedis__

#include <hiredis/hiredis.h>
#include "xRedisClient.h"

#include "ProtoCommon.h"

class CRTXRedis {

public:
    CRTXRedis(){}
    virtual ~CRTXRedis(){}

    void Init(const std::string& ip, int port);
    void Unin();

    bool Echo();
    bool SetSettingPush(const std::string& userid, int module, const std::string& field, const std::string& value);
    bool GetSettingPush(const std::string& userid, int module, const std::string& field, std::string& value);

private:
    std::string                      m_Ip;
    int                              m_Port;
    int                              m_RedisNodeNum;

    xRedisClient                     m_xRedisClient;
    RedisDBIdx*                      m_RedisDBIdx;
    RedisNode*                       m_RedisList;
};

#endif /* defined(__MsgServerConnector__CRTXRedis__) */
