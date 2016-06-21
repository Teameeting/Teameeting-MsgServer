//
//  SRTResponseCollection.h
//  MsgServerSequence
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerSequence__SRTResponseCollection__
#define __MsgServerSequence__SRTResponseCollection__

#include <stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <unordered_map>
#include <utility>
#include "sigslot.h"

#define REQUEST_TYPE_READ (1)
#define REQUEST_TYPE_WRITE (2)

#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"
#include "MsgServer/proto/storage_msg_type.pb.h"

class SRTRedisManager;

class SRTResponseCollection {
public:
    SRTResponseCollection(SRTRedisManager* manager, int reqType, int clientNum, const pms::StorageMsg& request, long long seqn);
    ~SRTResponseCollection();

    void AddResponse(const pms::StorageMsg& request, long long seqn);
    //const std::string& GetUserid() { return m_UserId; }

    class MsgSeqn {
        public:
        MsgSeqn(int clientNum, long long seqn, SRTRedisManager* manager, const pms::StorageMsg& request);
        ~MsgSeqn();
        int                 cnumber;
        int                 counter;
        SRTRedisManager*       rmanager;
        std::set<long long> seqns;
        pms::StorageMsg     storage;
        //pms::SeqnMsg
        sigslot::signal1<const std::string&> WriteResponse;
        sigslot::signal1<const std::string&> ReadResponse;
        bool AddAndCheckWrite(long long seq)
        {
            seqns.insert(seq);
            //printf("AddAndCheckWrite counter is:%d, cnumber is:%d\n", counter, cnumber);
            if ((++counter) == cnumber)
            {
                printf("write recv counter:%d, seqns.rbegin:%lld\n", counter, *seqns.rbegin());
                storage.set_sequence(*seqns.rbegin());
                WriteResponse(storage.SerializeAsString());
                //for(auto x : seqns)
                //{
                //     printf("seqn is:%lld\n", x);
                //}
                return true;
            } else {
                return false;
            }
        }
        bool AddAndCheckRead(long long seq)
        {
            seqns.insert(seq);
            printf("AddAndCheckRead counter is:%d, cnumber is:%d\n", counter, cnumber);
            if ((++counter) == cnumber)
            {
                printf("read recv counter:%d, seqns.rbegin:%lld\n", counter, *seqns.rbegin());
                storage.set_sequence(*seqns.rbegin());
                ReadResponse(storage.SerializeAsString());
                //for(auto x : seqns)
                //{
                //     printf("seqn is:%lld\n", x);
                //}
                return true;
            } else {
                return false;
            }
        }
    };
    //<msgid, msgseqn>
    typedef std::unordered_map<std::string, SRTResponseCollection::MsgSeqn*>  SeqnResponseMap;
    typedef SeqnResponseMap::iterator                 SeqnResponseMapIt;
    typedef SeqnResponseMap::const_iterator           SeqnResponseMapCIt;

private:

    int                 m_ClientNum;
    int                 m_ReqType;
    //std::string         m_UserId;
    SeqnResponseMap     m_WriteSeqnResponse;
    SeqnResponseMap     m_ReadSeqnResponse;
    SRTRedisManager*    m_pRedisManager;

};

#endif /* defined(__MsgServerSequence__SRTResponseCollection__) */
