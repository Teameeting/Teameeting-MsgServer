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

SRTResponseCollection::SRTResponseCollection(SRTRedisManager* manager, int reqType, int clientNum, const pms::StorageMsg& request, int64 seqn)
: m_pRedisManager(manager)
, m_ClientNum(clientNum)
, m_ReqType(reqType)
{
    AddResponse(request, seqn);
}

SRTResponseCollection::~SRTResponseCollection()
{

}

void SRTResponseCollection::AddResponse(const pms::StorageMsg& request, int64 seqn)
{
    printf("SRTResponseCollection::AddResponse new m_ReqType:%d, ruserid:%s, msgid:%s, seqn:%lld\n", m_ReqType, request.ruserid().c_str(), request.msgid().c_str(), seqn);
    if (m_ReqType==REQUEST_TYPE_READ)
    {
        SeqnResponseMapCIt cit = m_ReadSeqnResponse.find(request.msgid());
        if (cit != m_ReadSeqnResponse.end())
        {
            printf("SRTResponseCollection::AddResponse find msgid:%s\n", request.msgid().c_str());
            if (cit->second->AddAndCheckRead(seqn))
            {
                delete cit->second;
                m_ReadSeqnResponse.erase(cit);
            }
        } else {
            printf("SRTResponseCollection::AddResponse not find msgid:%s\n", request.msgid().c_str());
            m_ReadSeqnResponse.insert(make_pair(request.msgid(), new SRTResponseCollection::MsgSeqn(m_ClientNum, seqn, m_pRedisManager, request)));
        }
    } else if (m_ReqType==REQUEST_TYPE_WRITE)
    {
        SeqnResponseMapCIt cit = m_WriteSeqnResponse.find(request.msgid());
        if (cit != m_WriteSeqnResponse.end())
        {
            printf("SRTResponseCollection::AddResponse find msgid:%s\n", request.msgid().c_str());
            if (cit->second->AddAndCheckWrite(seqn))
            {
                delete cit->second;
                m_WriteSeqnResponse.erase(cit);
            }
        } else {
            printf("SRTResponseCollection::AddResponse not find msgid:%s\n", request.msgid().c_str());
            m_WriteSeqnResponse.insert(make_pair(request.msgid(), new SRTResponseCollection::MsgSeqn(m_ClientNum, seqn, m_pRedisManager, request)));
        }
    }
}


///////////////////inner class MsgSeqn//////////////////////////////////
///////////////////move here because//////////////////////////////////
///////////////////connect & disconnect//////////////////////////////////
///////////////////cannot be used in *.h//////////////////////////////////


SRTResponseCollection::MsgSeqn::MsgSeqn(int clientNum, int64 seqn, SRTRedisManager* manager, const pms::StorageMsg& request)
: cnumber(clientNum)
, counter(1)
, rmanager(manager)
, storage(request)
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

