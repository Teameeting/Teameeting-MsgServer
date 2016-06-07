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

#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

class SRTRedisManager;

class SRTResponseCollection {
public:
    SRTResponseCollection(SRTRedisManager* manager, int clientNum, const std::string& userid, const std::string& msgid, long long seqn);
    ~SRTResponseCollection();

    void AddResponse(const std::string& msgid, long long seqn);
    const std::string& GetUserid() { return m_UserId; }

    class MsgSeqn {
        public:
        MsgSeqn(int clientNum, long long seqn, const std::string& mid, SRTResponseCollection* coll, SRTRedisManager* manager);
        ~MsgSeqn();
        int                 cnumber;
        int                 counter;
        const std::string         msgid;
        SRTResponseCollection* pcoll;
        SRTRedisManager*       rmanager;
        std::set<long long> seqns;
        //<userid, msgid, seqn>
        //sigslot::signal3<const std::string&, const std::string&, long long> SequenceResponse;
        //<userid, pms::SeqnMsg
        //sigslot::signal2<const std::string&, const std::string&> SequenceResponse;
        //pms::SeqnMsg
        sigslot::signal1<const std::string&> SequenceResponse;
        bool AddAndCheckResp(long long seq)
        {
            seqns.insert(seq);
            //printf("AddAndCheckResp counter is:%d, cnumber is:%d\n", counter, cnumber);
            if ((++counter) == cnumber)
            {
                printf("seqn recv counter:%d, seqns.rbegin:%lld\n", counter, *seqns.rbegin());
                pms::StorageMsg msg;
                msg.set_userid(pcoll->GetUserid());
                msg.set_msgid(msgid);
                msg.set_sequence(*seqns.rbegin());
                SequenceResponse(msg.SerializeAsString());
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
    std::string         m_UserId;
    SeqnResponseMap     m_SeqnResponse;
    SRTRedisManager*    m_pRedisManager;

};

#endif /* defined(__MsgServerSequence__SRTResponseCollection__) */
