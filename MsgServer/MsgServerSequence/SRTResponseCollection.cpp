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

SRTResponseCollection::SRTResponseCollection(SRTRedisManager* manager, int reqType, int clientNum, const std::string& userid, const std::string& msgid, long long seqn)
: m_pRedisManager(manager)
, m_ClientNum(clientNum)
, m_ReqType(reqType)
, m_UserId(userid)
{
    AddResponse(msgid, seqn);
}

SRTResponseCollection::~SRTResponseCollection()
{

}

void SRTResponseCollection::AddResponse(const std::string& msgid, long long seqn)
{
    printf("SRTResponseCollection::AddResponse m_ReqType:%d\n", m_ReqType);
    if (m_ReqType==REQUEST_TYPE_READ)
    {
        SeqnResponseMapCIt cit = m_ReadSeqnResponse.find(msgid);
        if (cit != m_ReadSeqnResponse.end())
        {
            printf("SRTResponseCollection::AddResponse find msgid:%s\n", msgid.c_str());
            if (cit->second->AddAndCheckRead(seqn))
            {
                delete cit->second;
                m_ReadSeqnResponse.erase(cit);
                printf("after erase m_SeqnResponse.size:%d\n", m_ReadSeqnResponse.size());
            }
        } else {
            printf("SRTResponseCollection::AddResponse not find msgid:%s\n", msgid.c_str());
            m_ReadSeqnResponse.insert(make_pair(msgid, new SRTResponseCollection::MsgSeqn(m_ClientNum, seqn, msgid, this, m_pRedisManager)));
            printf("after insert m_ReadSeqnResponse.size:%d\n", m_ReadSeqnResponse.size());
        }
    } else if (m_ReqType==REQUEST_TYPE_WRITE)
    {
        SeqnResponseMapCIt cit = m_WriteSeqnResponse.find(msgid);
        if (cit != m_WriteSeqnResponse.end())
        {
            printf("SRTResponseCollection::AddResponse find msgid:%s\n", msgid.c_str());
            if (cit->second->AddAndCheckWrite(seqn))
            {
                delete cit->second;
                m_WriteSeqnResponse.erase(cit);
                printf("after erase m_WriteSeqnResponse.size:%d\n", m_WriteSeqnResponse.size());
            }
        } else {
            printf("SRTResponseCollection::AddResponse not find msgid:%s\n", msgid.c_str());
            m_WriteSeqnResponse.insert(make_pair(msgid, new SRTResponseCollection::MsgSeqn(m_ClientNum, seqn, msgid, this, m_pRedisManager)));
            printf("after insert m_WriteSeqnResponse.size:%d\n", m_WriteSeqnResponse.size());
        }
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
    WriteResponse.connect(rmanager, &SRTRedisManager::OnAddAndCheckWrite);
    ReadResponse.connect(rmanager, &SRTRedisManager::OnAddAndCheckRead);
}
SRTResponseCollection::MsgSeqn::~MsgSeqn()
{
    ReadResponse.disconnect(rmanager);
    WriteResponse.disconnect(rmanager);
    seqns.clear();
}

