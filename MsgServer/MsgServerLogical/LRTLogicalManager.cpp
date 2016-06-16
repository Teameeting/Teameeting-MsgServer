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

bool LRTLogicalManager::InsertMsg(LRTTransferSession* sess, pms::StorageMsg* storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("Insert map id is:%s\n", id);
    TransMsgInfo info;
    info.sess = sess;
    info.smsg = *storeMsg;
    {
        OSMutexLocker locker(&m_mutexMsg);
        //check if this id already in
        m_transMsgInfoMap.insert(std::make_pair(id, info));
    }
    return true;
}

bool LRTLogicalManager::UpdateMsg(pms::StorageMsg** storeMsg)
{
    if ((*storeMsg)->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", (*storeMsg)->userid().c_str(), (*storeMsg)->msgid().c_str());
    printf("UpdateMsg map id is:%s\n", id);

    {
        OSMutexLocker locker(&m_mutexMsg);
        TransMsgInfoMapIt it = m_transMsgInfoMap.find(id);
        if (it!=m_transMsgInfoMap.end())
        {
            // store sequence, get content, send to store
            it->second.smsg.set_sequence((*storeMsg)->sequence());
            (*storeMsg)->set_mtag(it->second.smsg.mtag());
            (*storeMsg)->set_content(it->second.smsg.content());
        } else {
            printf("ERROR HERE, UpdateMsg msg id:%s, userid:%s is not here\n", (*storeMsg)->msgid().c_str(), (*storeMsg)->userid().c_str());
            assert(false);
        }
    }
    return true;
}

bool LRTLogicalManager::RespAndDelMsg(pms::StorageMsg* storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    {
        OSMutexLocker locker(&m_mutexMsg);
        TransMsgInfoMapIt it = m_transMsgInfoMap.find(id);
        if (it!=m_transMsgInfoMap.end())
        {
            // after get from store, delete this msg in map
            printf("1, DeleteMsg m_transMsgInfoMap.size:%d\n", m_transMsgInfoMap.size());
            if (it->second.sess && it->second.sess->IsLiveSession())
            {
                it->second.sess->PushStoreMsg(*storeMsg);
            }
            m_transMsgInfoMap.erase(it);
            printf("2, DeleteMsg m_transMsgInfoMap.size:%d\n", m_transMsgInfoMap.size());
        } else {
            printf("ERROR HERE, DeleteMsg msg id:%s, userid:%s is not here\n", storeMsg->msgid().c_str(), storeMsg->userid().c_str());
            assert(false);
        }
    }
    return true;
}

bool LRTLogicalManager::GetMaxSeq(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("GetMaxSeq not implement now!!!\n");
    return true;
}

bool LRTLogicalManager::AddSeqReadMsg(LRTTransferSession* sess, pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("AddSeqReadMsg map id is:%s\n", id);
    TransMsgInfo info;
    info.sess = sess;
    info.smsg = *storeMsg;
    {
        OSMutexLocker locker(&m_mutexSeqReadMsg);
        //check if this id already in
        m_seqReadMsgInfoMap.insert(std::make_pair(id, info));
    }
    return true;
}

bool LRTLogicalManager::RespDelSeqMsg(pms::StorageMsg* storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("RespDelSeqMsg map id is:%s\n", id);
    {
        OSMutexLocker locker(&m_mutexSeqReadMsg);
        TransMsgInfoMapIt it = m_seqReadMsgInfoMap.find(id);
        if (it!=m_seqReadMsgInfoMap.end())
        {
            // after get from store, delete this msg in map
            printf("1, DeleteMsg m_seqReadMsgInfoMap.size:%d\n", m_seqReadMsgInfoMap.size());
            if (it->second.sess && it->second.sess->IsLiveSession())
            {
                it->second.sess->PushStoreMsg(*storeMsg);
            }
            m_seqReadMsgInfoMap.erase(it);
            printf("2, DeleteMsg m_seqReadMsgInfoMap.size:%d\n", m_seqReadMsgInfoMap.size());
        } else {
            printf("ERROR HERE, DeleteMsg userid:%s is not here\n", id);
            assert(false);
        }
    }
    return true;
}

bool LRTLogicalManager::SearchMsg(pms::StorageMsg* storeMsg)
{
    {
        OSMutexLocker locker(&m_mutexMsg);
    }
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
