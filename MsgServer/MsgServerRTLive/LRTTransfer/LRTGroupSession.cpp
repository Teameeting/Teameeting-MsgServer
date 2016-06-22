//
//  LRTGroupSession.cpp
//  dyncRTRTLive
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "LRTGroupSession.h"
#include "RTMessage.h"
#include "RTUtils.hpp"
#include "LRTConnManager.h"
#include "LRTRTLiveManager.h"

#define TIMEOUT_TS (60*1000)
#define MSG_PACKED_ONCE_NUM (10)

static int g_on_ticket_time = 0;

LRTGroupSession::LRTGroupSession()
: RTJSBuffer()
, LRTGrpConnTcp()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
, m_addr("")
, m_port(0)
, m_connectingStatus(0)
, m_wNewMsgId(0)
{
    AddObserver(this);
}

LRTGroupSession::~LRTGroupSession()
{
    DelObserver(this);
    Unit();
}

void LRTGroupSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(96L * 1024L);

    socket->SetTask(this);
    this->SetTimer(120*1000);
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedNewMsg.add_msgs();
    }
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedSeqnMsg.add_msgs();
    }
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedDataMsg.add_msgs();
    }

}

void LRTGroupSession::InitConf()
{
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedNewMsg.add_msgs();
    }
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedSeqnMsg.add_msgs();
    }
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedDataMsg.add_msgs();
    }
}

void LRTGroupSession::Unit()
{
    Disconn();
    m_connectingStatus = 0;
}

bool LRTGroupSession::Connect(const std::string addr, int port)
{
    m_addr = addr;
    m_port = port;
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

bool LRTGroupSession::Connect()
{
    if (m_addr.empty() || m_port < 2048) {
        LE("%s invalid params addr:%s, port:%d\n", __FUNCTION__, m_addr.c_str(), m_port);
        return false;
    }
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

void LRTGroupSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool LRTGroupSession::RefreshTime()
{
    UInt64 now = OS::Milliseconds();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        RTTcpNoTimeout::UpdateTimer();
        return true;
    } else {
        return false;
    }
}

void LRTGroupSession::KeepAlive()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MLIVE);
    c_msg.set_conn_tag(pms::EConnTag::TKEEPALIVE);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNOACK);
    t_msg.set_priority(pms::ETransferPriority::PNORMAL);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void LRTGroupSession::TestConnection()
{

}

void LRTGroupSession::EstablishConnection()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MLIVE);
    c_msg.set_conn_tag(pms::EConnTag::THI);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
    t_msg.set_priority(pms::ETransferPriority::PHIGH);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void LRTGroupSession::SendTransferData(const char* pData, int nLen)
{
    LRTRTLiveManager::Instance().SendResponseCounter();
    RTTcpNoTimeout::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void LRTGroupSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

// from RTTcpNoTimeout
void LRTGroupSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void LRTGroupSession::OnRecvMessage(const char*message, int nLen)
{
    //RTTransfer::DoProcessData(message, nLen);
    LRTGrpConnTcp::DoProcessData(message, nLen);
}

// process seqn read
void LRTGroupSession::OnWakeupEvent(const char*pData, int nLen)
{
    LI("LRTGroupSession::OnWakeupEvent was called\n");
    if (m_queueSeqnMsg.size()==0) return;
    bool hasData = false;
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
        if (m_queueSeqnMsg.size()>0)
        {
            hasData = true;
            m_packedSeqnMsg.mutable_msgs(i)->ParseFromString(m_queueSeqnMsg.front());
            {
                 OSMutexLocker locker(&m_mutexQueueSeqn);
                 m_queueSeqnMsg.pop();
            }
        } else {
             break;
        }
    }
    if (hasData)
    {
        pms::RelayMsg rmsg;
        rmsg.set_svr_cmds(pms::EServerCmd::CSYNCSEQN);
        rmsg.set_content(m_packedSeqnMsg.SerializeAsString());

        pms::TransferMsg tmsg;
        tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
        tmsg.set_flag(pms::ETransferFlag::FNOACK);
        tmsg.set_priority(pms::ETransferPriority::PNORMAL);
        tmsg.set_content(rmsg.SerializeAsString());

        LI("LRTGroupSession::OnWakeupEvent send seqn read request to logical server\n");
        this->SendTransferData(tmsg.SerializeAsString());
        for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
        {
             m_packedSeqnMsg.mutable_msgs(i)->Clear();
        }
    }
    if (m_queueSeqnMsg.size()>0)
    {
        this->Signal(Task::kWakeupEvent);
    }
}

// process new msgs
void LRTGroupSession::OnPushEvent(const char*pData, int nLen)
{
    LI("LRTGroupSession::OnPushEvent was called\n");
    if (m_queueNewMsg.size()==0) return;
    bool hasData = false;
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
        if (m_queueNewMsg.size()>0)
        {
            hasData = true;
            m_packedNewMsg.mutable_msgs(i)->ParseFromString(m_queueNewMsg.front());
            {
                 OSMutexLocker locker(&m_mutexQueueNew);
                 m_queueNewMsg.pop();
            }
        } else {
            break;
        }
    }
    if (hasData)
    {
        pms::RelayMsg rmsg;
        rmsg.set_svr_cmds(pms::EServerCmd::CNEWMSG);
        rmsg.set_content(m_packedNewMsg.SerializeAsString());

        pms::TransferMsg tmsg;
        tmsg.set_type(pms::ETransferType::TWRITE_REQUEST);
        tmsg.set_flag(pms::ETransferFlag::FNOACK);
        tmsg.set_priority(pms::ETransferPriority::PNORMAL);
        tmsg.set_content(rmsg.SerializeAsString());

        LI("LRTGroupSession::OnPushEvent send seqn write request to logical server\n");
        this->SendTransferData(tmsg.SerializeAsString());
        for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
        {
             m_packedNewMsg.mutable_msgs(i)->Clear();
        }
    }
    if (m_queueNewMsg.size()>0)
    {
        this->Signal(Task::kPushEvent);
    }
}

// process data read
void LRTGroupSession::OnTickEvent(const char*pData, int nLen)
{
    LI("LRTGroupSession::OnTickEvent was called, m_queueDataMsg.size:%d, g_on_ticket_time:%d\n"\
            , m_queueDataMsg.size(), ++g_on_ticket_time);
    if (m_queueDataMsg.size()==0) return;
    bool hasData = false;
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
        LI("LRTGroupSession::OnTickEvent loop m_queueDataMsg.size:%d\n", m_queueDataMsg.size());
        if (m_queueDataMsg.size()>0)
        {
            hasData = true;
            m_packedDataMsg.mutable_msgs(i)->ParseFromString(m_queueDataMsg.front());
            {
                 OSMutexLocker locker(&m_mutexQueueData);
                 m_queueDataMsg.pop();
            }
            printf("OnTickEvent read request sequence:%lld, userid:%s, m_queue.size:%d\n"\
                    , m_packedDataMsg.msgs(i).sequence()\
                    , m_packedDataMsg.msgs(i).userid().c_str()\
                    , m_queueDataMsg.size());
        } else {
            break;
        }
    }
    if (hasData)
    {
        pms::RelayMsg rmsg;
        rmsg.set_svr_cmds(pms::EServerCmd::CSYNCDATA);
        rmsg.set_content(m_packedDataMsg.SerializeAsString());

        pms::TransferMsg tmsg;
        tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
        tmsg.set_flag(pms::ETransferFlag::FNOACK);
        tmsg.set_priority(pms::ETransferPriority::PNORMAL);
        tmsg.set_content(rmsg.SerializeAsString());

        this->SendTransferData(tmsg.SerializeAsString());
        for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
        {
             m_packedDataMsg.mutable_msgs(i)->Clear();
        }
    }
    if (m_queueDataMsg.size()>0)
    {
        this->Signal(Task::kIdleEvent);
    }
}


// from LRTGrpConnTcp

void LRTGroupSession::OnLogin(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{

}

void LRTGroupSession::OnSndMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{

}

void LRTGroupSession::OnGetMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{

}

void LRTGroupSession::OnLogout(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{

}

void LRTGroupSession::OnKeepAlive(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{

}

void LRTGroupSession::OnSyncSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{

}

void LRTGroupSession::OnSyncData(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{

}

void LRTGroupSession::OnResponse(const char*pData, int nLen)
{
    RTTcpNoTimeout::SendTransferData(pData, nLen);
}

void LRTGroupSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        LRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

