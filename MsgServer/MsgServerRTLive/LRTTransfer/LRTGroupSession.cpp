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
    printf("LRTGroupSession was called\n");
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
        RTTcp::UpdateTimer();
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
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void LRTGroupSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

// from RTTcp
void LRTGroupSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void LRTGroupSession::OnRecvMessage(const char*message, int nLen)
{
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
    LI("%s was called\n", __FUNCTION__);
    pms::Login login;
    if (!login.ParseFromString(msg)) {
        LE("login.ParseFromString error\n");
    }

    m_userid = login.usr_from();
    m_token = login.usr_token();
    LI("Onlogin user:%s login\n", m_userid.c_str());
    std::string sid;
    {
        //store userid & pass
        LRTConnManager::ConnectionInfo* pci = new LRTConnManager::ConnectionInfo();
        if (pci) {
            GenericSessionId(sid);
            pci->_connId = sid;
            pci->_userId = m_userid;
            pci->_token = m_token;
            pci->_pConn = this;
            pci->_connType = pms::EConnType::TTCP;
            pci->_flag = 1;
            LRTConnManager::Instance().AddUser(pms::EConnType::TTCP, m_userid, pci);
            //LRTConnManager::Instance().ConnectionConnNotify(m_userId, m_token);

            // send response
            std::string resp;
            GenericResponse(pms::EServerCmd::CLOGIN, module, 0, resp);
            SendResponse(0, resp.c_str());
            m_login = true;
            return;
        } else {
            LE("new ConnectionInfo error userid:%s\n", m_userid.c_str());
            std::string resp;
            GenericResponse(pms::EServerCmd::CLOGIN, module, 101, resp);
            SendResponse(0, resp.c_str());
            m_login = false;
            return;
        }
    }
}

void LRTGroupSession::OnSndMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTGroupSession::OnGetMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTGroupSession::OnLogout(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
    LI("%s was called\n", __FUNCTION__);
    pms::Logout logout;
    if (!logout.ParseFromString(msg)) {
        LE("login.ParseFromString error\n");
    }
    assert(logout.usr_from().compare(m_userid)==0);
    LI("OnLogout user:%s logout\n", m_userid.c_str());
    assert(logout.usr_from().compare(m_userid)==0);
    std::string token;
    LRTConnManager::Instance().DelUser(pms::EConnType::TTCP, m_userid, token);
    //LRTConnManager::Instance().ConnectionLostNotify(m_userid, m_token);

    m_userid = "";
    m_token = "";
    std::string resp;
    GenericResponse(pms::EServerCmd::CLOGOUT, module, 0, resp);
    SendResponse(0, resp.c_str());
    m_login = false;
    return;
}

void LRTGroupSession::OnKeepAlive(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
    LI("%s was called\n", __FUNCTION__);
    if (m_login) {
        pms::Keep keep;
        if (!keep.ParseFromString(msg)) {
            LE("login.ParseFromString error\n");
        }
        LI("Userid:%s OnKeepAlive\n", keep.usr_from().c_str());
        RTTcp::UpdateTimer();
    }
}

void LRTGroupSession::OnSyncSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTGroupSession::OnSyncData(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTGroupSession::OnGroupNotify(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
    LI("%s was called\n", __FUNCTION__);
    pms::PackedStoreMsg packed;
    packed.ParseFromString(msg);
    for(int i=0;i<packed.msgs_size();++i)
    {
        if (packed.msgs(i).userid().length()==0)break;
        printf("LRTGroupSession::OnGroupNotify userid:%s, groupid:%s, svrcmd:%d, mflag:%d, sequence:%lld\n"\
                , packed.msgs(i).userid().c_str()\
                , packed.msgs(i).groupid().c_str()\
                , packed.msgs(i).svrcmd()\
                , packed.msgs(i).mflag()\
                , packed.msgs(i).sequence());

        pms::TransferMsg t_msg;
        pms::RelayMsg r_msg;
        pms::MsgRep resp;
        resp.set_svr_cmds(pms::EServerCmd::CGROUPNOTIFY);
        resp.set_mod_type(pms::EModuleType::TLIVE);
        resp.set_rsp_code(0);
        resp.set_rsp_cont(packed.msgs(i).SerializeAsString());

        // set relay
        r_msg.set_svr_cmds(pms::EServerCmd::CGROUPNOTIFY);
        r_msg.set_tr_module(pms::ETransferModule::MLIVE);
        r_msg.set_connector("");
        r_msg.set_content(resp.SerializeAsString());
        pms::ToUser *pto = new pms::ToUser;
        pto->add_users()->assign(packed.msgs(i).userid());
        r_msg.set_allocated_touser(pto);

        // set transfer
        t_msg.set_type(pms::ETransferType::TQUEUE);
        t_msg.set_content(r_msg.SerializeAsString());
        std::string response = t_msg.SerializeAsString();
        LRTConnManager::Instance().SendTransferData("", "", response);
    }
}


void LRTGroupSession::OnResponse(const char*pData, int nLen)
{
    LI("%s was called\n", __FUNCTION__);
    RTTcp::SendTransferData(pData, nLen);
}

void LRTGroupSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        LRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

void LRTGroupSession::GenericResponse(pms::EServerCmd cmd, pms::EModuleType module, int code, std::string& resp)
{
#if DEF_PROTO
    pms::MsgRep response;

    response.set_svr_cmds(cmd);
    response.set_mod_type(module);
    response.set_rsp_code(code);
    resp = response.SerializeAsString();

#else
    LE("not define DEF_PROTO\n");
#endif
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

