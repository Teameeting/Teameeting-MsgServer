//
//  RRTRTRedisManager.h
//  MsgServerRTRedis
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTRedis__RRTRTRedisManager__
#define __MsgServerRTRedis__RRTRTRedisManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "RRTTransferSession.h"
#include "RTMessage.h"
#include "RTTcp.h"
#include "RTType.h"
#include "RTSingleton.h"

#define DEF_PROTO 1
#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/meet_msg.pb.h"
#include "MsgServer/proto/meet_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"

#define HR_USERID       "hr_userid"
#define HR_CONNECTORID  "hr_connectorid"

class RRTRTRedisManager : public RTSingleton< RRTRTRedisManager >{
    friend class RTSingleton< RRTRTRedisManager >;
public:

    void PushBack(RRTTransferSession* session);
    void GenerateRTRedis();

    typedef std::list<RRTTransferSession*>      TransferSessionList;
    typedef TransferSessionList::iterator       TransferSessionListIt;

    bool    SignalKill();
    bool    ClearAll();

protected:
    RRTRTRedisManager() { }
    ~RRTRTRedisManager() { }
private:
};

#endif /* defined(__MsgServerRTRedis__RRTRTRedisManager__) */
