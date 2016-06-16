//
//  SRTRedisManager.h
//  MsgServerSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerSequence__SRTRedisManager__
#define __MsgServerSequence__SRTRedisManager__

#include <stdio.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include <queue>
#include "RTEventLooper.h"
#include "SRTSequenceRedis.h"
#include "sigslot.h"
#include "OSMutex.h"
#include "LinkedList.h"

#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

class SRTTransferSession;
class SRTResponseCollection;

class SRTRedisManager : public RTEventLooper
                      , public sigslot::has_slots<>{
public:
    SRTRedisManager();
    virtual ~SRTRedisManager();

    void Init(SRTTransferSession* sess);
    void Unin();

    void PostRedisRequest(const std::string& request);
    void PushRedisRequest(const std::string& request);

public:

    typedef struct _RedisGroup {
        std::string ip;
        int port;
        std::vector<SRTSequenceRedis*> redises;
    } RedisGroup;

    typedef std::unordered_map<std::string, RedisGroup*>  RedisGroupMgr;

    SRTSequenceRedis* LoopupForRedis(RedisGroup* group);
public:
    void OnWriteSeqn(const pms::StorageMsg& request, long long seqn);
    void OnAddAndCheckWrite(const std::string& msg);

    void OnReadSeqn(const pms::StorageMsg& request, long long seqn);
    void OnAddAndCheckRead(const std::string& msg);

// from RTEventLooper
public:
    virtual void OnPostEvent(const char*pData, int nSize) {}
    virtual void OnSendEvent(const void*pData, int nSize) {}
    virtual void OnWakeupEvent(const void*pData, int nSize);
    virtual void OnPushEvent(const char*pData, int nSize) {}
    virtual void OnTickEvent(const void*pData, int nSize);

private:
    int                                 m_RedisNum;
    SRTTransferSession*                 m_Session;
    std::vector<std::string>            m_RedisHosts;
    SInt64                              m_LastUpdateTime;
    RedisGroupMgr                       m_RedisGroupMgr;

    std::unordered_map<std::string, SRTResponseCollection*> m_ReadResponseCollections;
    std::unordered_map<std::string, SRTResponseCollection*> m_WriteResponseCollections;

    OSMutex                             m_MutexReadCollection;
    OSMutex                             m_MutexWriteCollection;

    std::queue<std::string>             m_SeqnResp4Read;
    std::queue<std::string>             m_SeqnResp4Write;

    OSMutex                             m_Mutex4Read;
    OSMutex                             m_Mutex4Write;

    int                                 m_PackedCounter;

    pms::PackedStoreMsg                 m_WritePackedMsg;
    pms::PackedStoreMsg                 m_ReadPackedMsg;
};

#endif /* defined(__MsgServerSequence__SRTRedisManager__) */
