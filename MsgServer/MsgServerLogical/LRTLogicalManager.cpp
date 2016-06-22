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



bool LRTLogicalManager::InsertDataWrite(LRTTransferSession* sess, pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("InsertDataWrite map id is:%s\n", id);
    TransMsgInfo info;
    info.sess = sess;
    info.smsg = *storeMsg;
    {
        OSMutexLocker locker(&m_mutexDataWrite);
        //check if this id already in
        m_dataWriteMap.insert(std::make_pair(id, info));
    }
    return true;
}

bool LRTLogicalManager::UpdateDataWrite(pms::StorageMsg** storeMsg)
{
    if ((*storeMsg)->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", (*storeMsg)->userid().c_str(), (*storeMsg)->msgid().c_str());
    printf("UpdateDataWrite map id is:%s\n", id);

    {
        OSMutexLocker locker(&m_mutexDataWrite);
        TransMsgInfoMapIt it = m_dataWriteMap.find(id);
        if (it!=m_dataWriteMap.end())
        {
            // store sequence, get content, send to store
            printf("UpdateDataWrite userid:%s, seqn:%lld, msgid:%s, content:%s\n"\
                    , (*storeMsg)->userid().c_str(), (*storeMsg)->sequence()\
                    , (*storeMsg)->msgid().c_str(), (*storeMsg)->content().c_str());

            it->second.smsg.set_sequence((*storeMsg)->sequence());
            it->second.smsg.set_msgid((*storeMsg)->msgid());
            (*storeMsg)->set_mtag(it->second.smsg.mtag());
            (*storeMsg)->set_content(it->second.smsg.content());
        } else {
            printf("ERROR HERE, UpdateDataWrite msg id:%s, userid:%s is not here\n", (*storeMsg)->msgid().c_str(), (*storeMsg)->userid().c_str());
            assert(false);
        }
    }
    return true;
}

bool LRTLogicalManager::DeleteDataWrite(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("DeleteDataWrite map id is:%s\n", id);
    {
        OSMutexLocker locker(&m_mutexDataWrite);
        TransMsgInfoMapIt it = m_dataWriteMap.find(id);
        if (it!=m_dataWriteMap.end())
        {
            // after get from store, delete this msg in map
            printf("1, DeleteDataWrite m_dataWriteMap.size:%d\n", m_dataWriteMap.size());
            if (it->second.sess && it->second.sess->IsLiveSession())
            {
                it->second.sess->PushWriteMsg(*storeMsg);
            }
            m_dataWriteMap.erase(it);
            printf("2, DeleteDataWrite m_dataWriteMap.size:%d\n", m_dataWriteMap.size());
        } else {
            printf("ERROR HERE, DeleteDataWrite msg id:%s, userid:%s is not here\n", storeMsg->msgid().c_str(), storeMsg->userid().c_str());
            assert(false);
        }
    }
    return true;
}


bool LRTLogicalManager::InsertDataRead(LRTTransferSession* sess, pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("InsertDataRead map id is:%s\n", id);
    TransMsgInfo info;
    info.sess = sess;
    info.smsg = *storeMsg;
    {
        OSMutexLocker locker(&m_mutexDataRead);
        //check if this id already in
        m_dataReadMap.insert(std::make_pair(id, info));
    }
    return true;
}

bool LRTLogicalManager::UpdateDataRead(pms::StorageMsg** storeMsg)
{
    printf("UpdateDataRead NOT implement!!!\n\n");

    return true;
}

bool LRTLogicalManager::DeleteDataRead(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("DeleteDataRead map id is:%s\n", id);
    {
        OSMutexLocker locker(&m_mutexDataRead);
        TransMsgInfoMapIt it = m_dataReadMap.find(id);
        if (it!=m_dataReadMap.end())
        {
            // after get from store, delete this msg in map
            printf("1, DeleteDataRead m_dataReadMap.size:%d\n", m_dataReadMap.size());
            if (it->second.sess && it->second.sess->IsLiveSession())
            {
                it->second.sess->PushReadMsg(*storeMsg);
            }
            m_dataReadMap.erase(it);
            printf("2, DeleteDataRead m_dataReadMap.size:%d\n", m_dataReadMap.size());
        } else {
            printf("ERROR HERE, DeleteDataRead msg id:%s, userid:%s is not here\n", storeMsg->msgid().c_str(), storeMsg->userid().c_str());
            assert(false);
        }
    }
    return true;
}


bool LRTLogicalManager::InsertSeqnRead(LRTTransferSession* sess, pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("InsertSeqnRead map id is:%s\n", id);
    TransMsgInfo info;
    info.sess = sess;
    info.smsg = *storeMsg;
    {
        OSMutexLocker locker(&m_mutexSeqnRead);
        //check if this id already in
        m_seqnReadMap.insert(std::make_pair(id, info));
    }
    return true;
}

bool LRTLogicalManager::UpdateSeqnRead(pms::StorageMsg** storeMsg)
{
    printf("UpdateSeqnRead NOT implement!!!\n\n");
    return true;

}

bool LRTLogicalManager::DeleteSeqnRead(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->userid().c_str(), storeMsg->msgid().c_str());
    printf("DeleteSeqnRead map id is:%s\n", id);
    {
        OSMutexLocker locker(&m_mutexSeqnRead);
        TransMsgInfoMapIt it = m_seqnReadMap.find(id);
        if (it!=m_seqnReadMap.end())
        {
            // after get from store, delete this msg in map
            printf("1, DeleteSeqnRead m_seqnReadMap.size:%d\n", m_seqnReadMap.size());
            if (it->second.sess && it->second.sess->IsLiveSession())
            {
                it->second.sess->PushReadMsg(*storeMsg);
            }
            m_seqnReadMap.erase(it);
            printf("2, DeleteSeqnRead m_seqnReadMap.size:%d\n", m_seqnReadMap.size());
        } else {
            printf("ERROR HERE, DeleteSeqnRead userid:%s is not here\n", id);
            assert(false);
        }
    }
    return true;
}

bool LRTLogicalManager::GetSessFromId(const std::string& userid, const std::string& msgid, LRTTransferSession** sess)
{
    assert(userid.length()>0);
    assert(msgid.length()>0);
    assert(sess);
    char id[1024] = {0};
    sprintf(id, "%s:%s", userid.c_str(), msgid.c_str());
    printf("GetSessFromMsg map id is:%s\n", id);
    {
        OSMutexLocker locker(&m_mutexSeqnRead);
        TransMsgInfoMapIt it = m_seqnReadMap.find(id);
        if (it!=m_seqnReadMap.end())
        {
            *sess = it->second.sess;
        } else {
            printf("ERROR HERE, GetSessFromMsg userid:%s is not here\n", id);
            assert(false);
        }
    }
    return true;
}

bool LRTLogicalManager::ReadLocalSeqn(pms::StorageMsg*  storeMsg, long long* seqn)
{
    if (storeMsg->userid().length()==0) return false;
    printf("ReadLocalSeqn userid is:%s\n", storeMsg->userid().c_str());
    bool found = false;
    {
        OSMutexLocker locker(&m_mutexLocalSeqn);
        UserLocalSeqnMapIt it = m_localSeqnMap.find(storeMsg->userid());
        if (it!=m_localSeqnMap.end())
        {
            *seqn = it->second;
            found = true;
        } else {
            found = false;
        }
    }
    return found;
}

bool LRTLogicalManager::UpdateLocalSeqn(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    printf("UpdateLocalSeqn userid is:%s\n", storeMsg->userid().c_str());
    {
        OSMutexLocker locker(&m_mutexLocalSeqn);
        UserLocalSeqnMapIt it = m_localSeqnMap.find(storeMsg->userid());
        if (it!=m_localSeqnMap.end())
        {
            assert(storeMsg->sequence() > it->second);
            it->second = storeMsg->sequence();
        } else {
            m_localSeqnMap.insert(std::make_pair(storeMsg->userid(), storeMsg->sequence()));
        }
        printf("UpdateLocalSeqn m_localSeqnMap.size:%d\n", m_localSeqnMap.size());
    }
    return true;
}

bool LRTLogicalManager::UpdateLocalMaxSeqn(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->userid().length()==0) return false;
    printf("UpdateLocalSeqn userid is:%s\n", storeMsg->userid().c_str());
    {
        OSMutexLocker locker(&m_mutexLocalSeqn);
        UserLocalSeqnMapIt it = m_localSeqnMap.find(storeMsg->userid());
        if (it!=m_localSeqnMap.end())
        {
            assert(storeMsg->maxseqn() > it->second);
            it->second = storeMsg->maxseqn();
        } else {
            m_localSeqnMap.insert(std::make_pair(storeMsg->userid(), storeMsg->maxseqn()));
        }
        printf("UpdateLocalSeqn m_localSeqnMap.size:%d\n", m_localSeqnMap.size());
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
