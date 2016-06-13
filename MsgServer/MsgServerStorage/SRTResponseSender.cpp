//
//  SRTResponseSender.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTResponseSender.h"
#include "SRTTransferSession.h"

#define PACKED_MSG_ONCE_NUM (10)

static int g_push_response_counter = 0;
static int g_idle_event_counter = 0;

SRTResponseSender::SRTResponseSender()
{

}

SRTResponseSender::~SRTResponseSender()
{

}

void SRTResponseSender::Init(SRTTransferSession *sess)
{
    m_IsRun = 1;
    m_TransferSession = sess;
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_SendPackedMsg.add_msgs();
    }
    printf("SRTResponseSender::Init was called...\n");
}

void SRTResponseSender::Unin()
{
    m_IsRun = 0;
    m_TransferSession = nullptr;
    while(!m_QSendMsg.empty())
    {
        m_QSendMsg.pop();
    }
}

void SRTResponseSender::PushResponseData(const char*pData, int nSize)
{
    //printf("SRTResponseSender::PushResponseData g_push_response_counter:%d, QSendMsg.size:%d\n",  ++g_push_response_counter, m_QSendMsg.size());
    {
        OSMutexLocker locker(&m_Mutex2Send);
        std::string str(pData, nSize);
        m_QSendMsg.push(str);
    }
    this->Signal(Task::kIdleEvent);
}

void SRTResponseSender::OnPostEvent(const char*pData, int nSize)
{

}

void SRTResponseSender::OnWakeupEvent(const void*pData, int nSize)
{

}

void SRTResponseSender::OnPushEvent(const char*pData, int nSize)
{

}

void SRTResponseSender::OnTickEvent(const void*pData, int nSize)
{
#if 1
    if (!m_IsRun) return;
    if(m_QSendMsg.size()==0) return;
    bool hasData = false;
    //printf("SRTResponseSender::OnTickEvent g_idle_event_counter:%d, m_QSendMsg.size:%d\n",  ++g_idle_event_counter, m_QSendMsg.size());
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_QSendMsg.size()>0)
        {
            hasData = true;
            m_SendPackedMsg.mutable_msgs(i)->ParseFromString(m_QSendMsg.front());
            {
                OSMutexLocker locker(&m_Mutex2Send);
                m_QSendMsg.pop();
            }
        }
    }
    if (hasData)
    {
        if (m_TransferSession && m_TransferSession->IsLiveSession())
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TPUSH);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(m_SendPackedMsg.SerializeAsString());
            m_TransferSession->SendTransferData(tmsg.SerializeAsString());
        }
        for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
            m_SendPackedMsg.mutable_msgs(i)->Clear();
        }
    }
#endif
}

///////////////////////////////////////////////////////
