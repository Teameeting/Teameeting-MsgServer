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
#include "RTEventLooper.h"
#include "SRTSequenceRedis.h"
#include "sigslot.h"
#include "OSMutex.h"
#include "LinkedList.h"

#define DEF_PROTO 1
#include "MsgServer/proto/sequence_msg.pb.h"

class SRTTransferSession;
class SRTResponseCollection;

class SRTRedisManager : public RTEventLooper
                      , public sigslot::has_slots<>{
public:
    SRTRedisManager();
    virtual ~SRTRedisManager();

    void Init(SRTTransferSession* sess);
    void Unin();

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
    void OnRequestSeqn(const std::string& userid, const std::string& msgid, long long seqn);
    //void OnAddAndCheckSeqn(const std::string& userid, const std::string& msgid, long long seqn);
    void OnAddAndCheckSeqn(const std::string& msg);

// from RTEventLooper
public:
    virtual void OnPostEvent(const char*pData, int nSize);
    virtual void OnSendEvent(const void*pData, int nSize) {}
    virtual void OnWakeupEvent(const void*pData, int nSize){}
    virtual void OnPushEvent(const char*pData, int nSize);
    virtual void OnTickEvent(const void*pData, int nSize);

private:
    int                                 m_RedisNum;
    SRTTransferSession*                 m_Session;
    std::vector<std::string>            m_RedisHosts;
    SInt64                              m_LastUpdateTime;
    RedisGroupMgr                       m_RedisGroupMgr;
    std::unordered_map<std::string, SRTResponseCollection*> m_ResponseCollections;
    OSMutex                             m_MutexCollection;
    List                                m_SeqnResp2Send;
    OSMutex                             m_Mutex2Send;
    int                                 m_PackedCounter;
    std::vector<pms::PackedSeqnMsg*>    m_PackedSeqnMsgs;
};

#endif /* defined(__MsgServerSequence__SRTRedisManager__) */
