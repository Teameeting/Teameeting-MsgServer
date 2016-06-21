//
//  LRTRTLiveManager.cpp
//  dyncRTRTLive
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "LRTRTLiveManager.h"
#include <assert.h>
#include <algorithm>
#include "MsgServer/MSCommon/MSBase/RTHiredis.h"
#include "LRTTransferSession.h"
#include "OS.h"


#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

#define MAX_PROCESS_MSG_ONCE 200

static long long gRecvCounter = 0;
static long long gSendCounter = 0;
static SInt64 gLastRecvTime = 0;
static SInt64 gLastSendTime = 0;

bool LRTRTLiveManager::RecvRequestCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:gRecvCounter:%lld\n", curTime, ++gRecvCounter);
    //LI("%s", buf);
    memset(buf, 0, 128);
    return false;
}

bool LRTRTLiveManager::SendResponseCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "send_time:%lld:gSendCounter:%lld\n", curTime, ++gSendCounter);
    //LI("%s", buf);
    memset(buf, 0, 128);
    return false;
}

bool LRTRTLiveManager::SignalKill()
{
    return true;
}

bool LRTRTLiveManager::ClearAll()
{
    return true;
}
