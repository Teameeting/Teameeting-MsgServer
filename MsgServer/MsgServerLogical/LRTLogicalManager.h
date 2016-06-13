//
//  LRTLogicalManager.h
//  MsgServerLogical
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerLogical__LRTLogicalManager__
#define __MsgServerLogical__LRTLogicalManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include "RTType.h"
#include "RTSingleton.h"

#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

class LRTTransferSession;

class LRTLogicalManager : public RTSingleton< LRTLogicalManager >{
    friend class RTSingleton< LRTLogicalManager >;
public:

    typedef struct TransfMsgInfo {
        LRTTransferSession*     sess;
        pms::StorageMsg         smsg;
    } TransMsgInfo;

    bool RecvRequestCounter();
    bool SendResponseCounter();

    bool InitManager()
    {

    }

    bool UninManager()
    {
        m_RedisHosts.clear();
    }

    void PushRedisHosts(std::string host)
    {
        m_RedisHosts.push_back(host);
    }

    void GetRedisHosts(std::vector<std::string>* hosts)
    {
         for(std::vector<std::string>::iterator it=m_RedisHosts.begin();it!=m_RedisHosts.end();++it)
         {
             hosts->push_back(*it);
         }
    }

    bool InsertMsg(LRTTransferSession* sess, pms::StorageMsg*  storeMsg);
    bool UpdateMsg(pms::StorageMsg** storeMsg);
    bool RespAndDelMsg(pms::StorageMsg*  storeMsg);
    bool SearchMsg(pms::StorageMsg*  storeMsg);
    bool GetMaxSeq(pms::StorageMsg*  storeMsg);

    bool AddSeqReadMsg(LRTTransferSession* sess, pms::StorageMsg*  storeMsg);
    bool RespDelSeqMsg(pms::StorageMsg* storeMsg);

    bool    SignalKill();
    bool    ClearAll();
protected:
    LRTLogicalManager(){}

    ~LRTLogicalManager(){}
protected:
    typedef std::unordered_map<std::string, TransMsgInfo>       TransMsgInfoMap;
    typedef TransMsgInfoMap::iterator                           TransMsgInfoMapIt;

private:
    std::vector<std::string>            m_RedisHosts;
    OSMutex                             m_mutexMsg;
    OSMutex                             m_mutexSeqReadMsg;
    TransMsgInfoMap                     m_transMsgInfoMap;
    TransMsgInfoMap                     m_seqReadMsgInfoMap;
};

#endif /* defined(__MsgServerLogical__LRTLogicalManager__) */
