//
//  SRTStorageManager.cpp
//  dyncRTStorage
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTStorageManager.h"
#include <assert.h>
#include <algorithm>
#include "MsgServer/MSCommon/MSBase/RTHiredis.h"
#include "SRTTransferSession.h"
#include "SRTStorageGenerator.h"
#include "OS.h"


#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

#define MAX_PROCESS_MSG_ONCE 200

static long long gRecvCounter = 0;
static long long gSendCounter = 0;

bool SRTStorageManager::RecvRequestCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:gRecvCounter:%lld\n", curTime, ++gRecvCounter);
    fwrite(buf, 1, 128, m_RecvFile);
    //LI("%s\n", buf);
    return false;
}

bool SRTStorageManager::SendResponseCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "send_time:%lld:gSendCounter:%lld\n", curTime, ++gSendCounter);
    fwrite(buf, 1, 128, m_SendFile);
    //LI("%s\n", buf);
    return false;
}

bool SRTStorageManager::SignalKill()
{
    return true;
}

bool SRTStorageManager::ClearAll()
{
    return true;
}
