//
//  LRTLogicalManager.cpp
//  dyncRTLogical
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "LRTLogicalManager.h"
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

bool LRTLogicalManager::RecvRequestCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:gRecvCounter:%lld\n", curTime, ++gRecvCounter);
    LI("%s", buf);
    memset(buf, 0, 128);
    return false;
}

bool LRTLogicalManager::SendResponseCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "send_time:%lld:gSendCounter:%lld\n", curTime, ++gSendCounter);
    LI("%s", buf);
    memset(buf, 0, 128);
    return false;
}

bool LRTLogicalManager::InsertMsg(pms::StorageMsg* storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("Insert map id is:%s\n", id);
    m_requestMsgMap.insert(std::make_pair(id, *storeMsg));

    //std::string id=storeMsg->userid();
    //id.append(":");
    //id.append(storeMsg->msgid());
    //printf("Insert map id is:%s\n", id.c_str());
    //m_requestMsgMap.insert(std::make_pair(id, *storeMsg));
    return true;
}

bool LRTLogicalManager::UpdateMsg(pms::StorageMsg** storeMsg)
{
    if ((*storeMsg)->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", (*storeMsg)->userid().c_str(), (*storeMsg)->msgid().c_str());
    printf("UpdateMsg map id is:%s\n", id);

    //std::string id=(*storeMsg)->userid();
    //id.append(":");
    //id.append((*storeMsg)->msgid());
    //printf("UpdateMsg map id is:%s\n", id.c_str());
    RequestMsgMapIt it = m_requestMsgMap.find(id);
    if (it!=m_requestMsgMap.end())
    {
        // store sequence, get content, send to store
        it->second.set_sequence((*storeMsg)->sequence());
        (*storeMsg)->set_mflag(it->second.mflag());
        (*storeMsg)->set_content(it->second.content());
    } else {
        printf("ERROR HERE, UpdateMsg msg id:%s, userid:%s is not here\n", (*storeMsg)->msgid().c_str(), (*storeMsg)->userid().c_str());
        assert(false);
    }
    return true;
}

bool LRTLogicalManager::DeleteMsg(pms::StorageMsg* storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    RequestMsgMapIt it = m_requestMsgMap.find(id);
    if (it!=m_requestMsgMap.end())
    {
        // after get from store, delete this msg in map
        printf("1, DeleteMsg m_requestMsgMap.size:%d\n", m_requestMsgMap.size());
        m_requestMsgMap.erase(it);
        printf("2, DeleteMsg m_requestMsgMap.size:%d\n", m_requestMsgMap.size());
    } else {
        printf("ERROR HERE, DeleteMsg msg id:%s, userid:%s is not here\n", storeMsg->msgid().c_str(), storeMsg->userid().c_str());
        assert(false);
    }
    return true;
}

bool LRTLogicalManager::SearchMsg(pms::StorageMsg* storeMsg)
{
    return true;
}


bool LRTLogicalManager::SignalKill()
{
    return true;
}

bool LRTLogicalManager::ClearAll()
{
    return true;
}
