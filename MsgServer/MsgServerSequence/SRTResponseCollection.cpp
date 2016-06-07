//
//  SRTResponseCollection.cpp
//  dyncRTSequence
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTResponseCollection.h"
#include "SRTRedisManager.h"


#define DEF_PROTO 1
#include "MsgServer/proto/storage_msg.pb.h"

SRTResponseCollection::SRTResponseCollection(SRTRedisManager* manager, int clientNum, const std::string& userid, const std::string& msgid, long long seqn)
: m_pRedisManager(manager)
, m_ClientNum(clientNum)
{
    m_UserId = userid;
    AddResponse(msgid, seqn);
}

SRTResponseCollection::~SRTResponseCollection()
{

}

void SRTResponseCollection::AddResponse(const std::string& msgid, long long seqn)
{
    SeqnResponseMapCIt cit = m_SeqnResponse.find(msgid);
    if (cit != m_SeqnResponse.end())
    {
        //printf("SRTResponseCollection::AddResponse find msgid:%s\n", msgid.c_str());
        if (cit->second->AddAndCheckResp(seqn))
        {
             delete cit->second;
             m_SeqnResponse.erase(cit);
             //printf("after erase m_SeqnResponse.size:%d\n", m_SeqnResponse.size());
        }
    } else {
        //printf("SRTResponseCollection::AddResponse not find msgid:%s\n", msgid.c_str());
        m_SeqnResponse.insert(make_pair(msgid, new SRTResponseCollection::MsgSeqn(m_ClientNum, seqn, msgid, this, m_pRedisManager)));
        //printf("after insert m_SeqnResponse.size:%d\n", m_SeqnResponse.size());
    }
}

///////////////////inner class MsgSeqn//////////////////////////////////
///////////////////move here because//////////////////////////////////
///////////////////connect & disconnect//////////////////////////////////
///////////////////cannot be used in *.h//////////////////////////////////


SRTResponseCollection::MsgSeqn::MsgSeqn(int clientNum, long long seqn, const std::string& mid, SRTResponseCollection* coll, SRTRedisManager* manager)
: cnumber(clientNum)
, counter(1)
, msgid(mid)
, pcoll(coll)
, rmanager(manager)
{
    seqns.insert(seqn);
    SequenceResponse.connect(rmanager, &SRTRedisManager::OnAddAndCheckSeqn);
}
SRTResponseCollection::MsgSeqn::~MsgSeqn()
{
    seqns.clear();
    SequenceResponse.disconnect(rmanager);
}

