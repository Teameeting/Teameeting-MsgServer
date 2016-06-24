//
//  SRTSequenceManager.cpp
//  dyncRTSequence
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTSequenceManager.h"
#include <assert.h>
#include <algorithm>
#include "SRTTransferSession.h"
#include "SRTSequenceGenerator.h"
#include "OS.h"


#define DEF_PROTO 1

#define MAX_PROCESS_MSG_ONCE 200

static long long gRecvCounter = 0;
static long long gSendCounter = 0;

bool SRTSequenceManager::RecvRequestCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:gRecvCounter:%lld\n", curTime, ++gRecvCounter);
    //fwrite(buf, 1, 128, m_RecvFile);
    LI("%s\n", buf);
    return false;
}

bool SRTSequenceManager::SendResponseCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "send_time:%lld:gSendCounter:%lld\n", curTime, ++gSendCounter);
    //fwrite(buf, 1, 128, m_SendFile);
    LI("%s\n", buf);
    return false;
}

bool SRTSequenceManager::SignalKill()
{
    return true;
}

bool SRTSequenceManager::ClearAll()
{
    return true;
}
