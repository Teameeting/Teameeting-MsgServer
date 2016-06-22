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

    bool InsertDataWrite(LRTTransferSession* sess, pms::StorageMsg*  storeMsg);
    bool UpdateDataWrite(pms::StorageMsg** storeMsg);
    bool DeleteDataWrite(pms::StorageMsg*  storeMsg);

    bool InsertDataRead(LRTTransferSession* sess, pms::StorageMsg*  storeMsg);
    bool UpdateDataRead(pms::StorageMsg** storeMsg);
    bool DeleteDataRead(pms::StorageMsg*  storeMsg);

    bool InsertSeqnRead(LRTTransferSession* sess, pms::StorageMsg*  storeMsg);
    bool UpdateSeqnRead(pms::StorageMsg** storeMsg);
    bool DeleteSeqnRead(pms::StorageMsg*  storeMsg);
    bool GetSessFromId(const std::string& userid, const std::string& msgid, LRTTransferSession** sess);


    bool ReadLocalSeqn(pms::StorageMsg*  storeMsg, long long* seqn);
    bool UpdateLocalSeqn(pms::StorageMsg*  storeMsg);
    bool UpdateLocalMaxSeqn(pms::StorageMsg*  storeMsg);

    bool    SignalKill();
    bool    ClearAll();
protected:
    LRTLogicalManager(){}

    ~LRTLogicalManager(){}
protected:
    typedef std::unordered_map<std::string, TransMsgInfo>       TransMsgInfoMap;
    typedef TransMsgInfoMap::iterator                           TransMsgInfoMapIt;

    typedef std::unordered_map<std::string, long long>          UserLocalSeqnMap;
    typedef UserLocalSeqnMap::iterator                           UserLocalSeqnMapIt;

private:
    std::vector<std::string>            m_RedisHosts;
    OSMutex                             m_mutexDataWrite;
    OSMutex                             m_mutexDataRead;
    OSMutex                             m_mutexSeqnRead;

    OSMutex                             m_mutexLocalSeqn;

    TransMsgInfoMap                     m_dataWriteMap;
    TransMsgInfoMap                     m_dataReadMap;
    TransMsgInfoMap                     m_seqnReadMap;

    UserLocalSeqnMap                    m_localSeqnMap;
};

#endif /* defined(__MsgServerLogical__LRTLogicalManager__) */
