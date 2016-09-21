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
    AddObserver(this);
}

SRTResponseSender::~SRTResponseSender()
{
    DelObserver(this);
}

void SRTResponseSender::Init(SRTTransferSession *sess)
{
    m_IsRun = 1;
    m_TransferSession = sess;
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_SendPushMsg.add_msgs();
    }
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_SendPostMsg.add_msgs();
    }
    LI("SRTResponseSender::Init was called...\n");
}

void SRTResponseSender::Unin()
{
    m_IsRun = 0;
    m_TransferSession = nullptr;
    while(!m_QPostMsg.empty())
    {
        m_QPostMsg.pop();
    }
    while(!m_QPushMsg.empty())
    {
        m_QPushMsg.pop();
    }
}

void SRTResponseSender::PushResponseData(const char*pData, int nSize)
{
    //LI("SRTResponseSender::PushResponseData g_push_response_counter:%d, QPushMsg.size:%d\n",  ++g_push_response_counter, m_QPushMsg.size());
    {
        OSMutexLocker locker(&m_MutexPush);
        std::string str(pData, nSize);
        m_QPushMsg.push(str);
    }
    this->Signal(Task::kIdleEvent);
}

void SRTResponseSender::PostResponseData(const char*pData, int nSize)
{
    //LI("SRTResponseSender::PostResponseData g_post_response_counter:%d, QPostMsg.size:%d\n",  ++g_post_response_counter, m_QPostMsg.size());
    {
        OSMutexLocker locker(&m_MutexPost);
        std::string str(pData, nSize);
        m_QPostMsg.push(str);
    }
    this->Signal(Task::kWakeupEvent);
}

void SRTResponseSender::OnWakeupEvent(const void*pData, int nSize)
{
#if 1
    if (!m_IsRun) return;
    if(m_QPostMsg.size()==0) return;
    bool hasData = false;
    //LI("SRTResponseSender::OnWakeupEvent g_wakeup_event_counter:%d, m_QPostMsg.size:%d\n",  ++g_wakeup_event_counter, m_QPostMsg.size());
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_QPostMsg.size()>0)
        {
            hasData = true;
            m_SendPostMsg.mutable_msgs(i)->ParseFromString(m_QPostMsg.front());
            {
                OSMutexLocker locker(&m_MutexPost);
                m_QPostMsg.pop();
            }
        } else {
            break;
        }
    }
    if (hasData)
    {
        if (m_TransferSession && m_TransferSession->IsLiveSession())
        {
            pms::RelayMsg rmsg;
            // this mean it is about data, not seqn
            // because this is all read
            rmsg.set_svr_cmds(pms::EServerCmd::CDATA);
            rmsg.set_content(m_SendPostMsg.SerializeAsString());

            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TREAD_RESPONSE);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(rmsg.SerializeAsString());

            m_TransferSession->SendTransferData(tmsg.SerializeAsString());
        }
        for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
            m_SendPostMsg.mutable_msgs(i)->Clear();
        }
    }
    if(m_QPostMsg.size()>0)
    {
        this->Signal(Task::kWakeupEvent);
    }
#endif
}

void SRTResponseSender::OnTickEvent(const void*pData, int nSize)
{
#if 1
    if (!m_IsRun) return;
    if(m_QPushMsg.size()==0) return;
    bool hasData = false;
    //LI("SRTResponseSender::OnTickEvent g_idle_event_counter:%d, m_QSendMsg.size:%d\n",  ++g_idle_event_counter, m_QPushMsg.size());
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_QPushMsg.size()>0)
        {
            hasData = true;
            m_SendPushMsg.mutable_msgs(i)->ParseFromString(m_QPushMsg.front());
            {
                OSMutexLocker locker(&m_MutexPush);
                m_QPushMsg.pop();
            }
        } else {
             break;
        }
    }
    if (hasData)
    {
        if (m_TransferSession && m_TransferSession->IsLiveSession())
        {
            pms::RelayMsg rmsg;
            // this mean it is about data, not seqn
            // because this is all write
            rmsg.set_svr_cmds(pms::EServerCmd::CDATA);
            rmsg.set_content(m_SendPushMsg.SerializeAsString());

            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TWRITE_RESPONSE);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(rmsg.SerializeAsString());

            m_TransferSession->SendTransferData(tmsg.SerializeAsString());
        }
        for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
            m_SendPushMsg.mutable_msgs(i)->Clear();
        }
    }
    if(m_QPushMsg.size()>0)
    {
        this->Signal(Task::kIdleEvent);
    }
#endif
}

void SRTResponseSender::ConnectionDisconnected()
{
    LI("%s was called\n", __FUNCTION__);
}

///////////////////////////////////////////////////////
