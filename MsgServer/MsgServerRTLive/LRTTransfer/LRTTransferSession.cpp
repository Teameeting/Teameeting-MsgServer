//
//  LRTTransferSession.cpp
//  dyncRTRTLive
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "LRTTransferSession.h"
#include "RTMessage.h"
#include "RTUtils.hpp"
#include "LRTConnManager.h"
#include "LRTRTLiveManager.h"

#define TIMEOUT_TS (60*1000)
#define MSG_PACKED_ONCE_NUM (10)

LRTTransferSession::LRTTransferSession()
: RTJSBuffer()
, RTTransfer()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
, m_addr("")
, m_port(0)
, m_connectingStatus(0)
{
    AddObserver(this);
}

LRTTransferSession::~LRTTransferSession()
{
    DelObserver(this);
    Unit();
}

void LRTTransferSession::Init()
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
         m_packedSeqnMsg.add_msgs();
    }
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedDataMsg.add_msgs();
    }

}

void LRTTransferSession::InitConf()
{
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedSeqnMsg.add_msgs();
    }
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
         m_packedDataMsg.add_msgs();
    }
}

void LRTTransferSession::Unit()
{
    Disconn();
    m_connectingStatus = 0;
}

bool LRTTransferSession::Connect(const std::string addr, int port)
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

bool LRTTransferSession::Connect()
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

void LRTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool LRTTransferSession::RefreshTime()
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

void LRTTransferSession::KeepAlive()
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

void LRTTransferSession::TestConnection()
{

}

void LRTTransferSession::EstablishConnection()
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

void LRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    LRTRTLiveManager::Instance().SendResponseCounter();
    RTTcpNoTimeout::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void LRTTransferSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

// from RTTcpNoTimeout
void LRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void LRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
    RTTransfer::DoProcessData(message, nLen);
}

// process seqn read
void LRTTransferSession::OnWakeupEvent(const char*pData, int nLen)
{
    LI("LRTTransferSession::OnWakeupEvent was called\n");
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

        LI("LRTTransferSession::OnWakeupEvent send seqn read request to logical server\n");
        this->SendTransferData(tmsg.SerializeAsString());
        for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
        {
             m_packedSeqnMsg.mutable_msgs(i)->Clear();
        }
    }
}

// process data read
void LRTTransferSession::OnTickEvent(const char*pData, int nLen)
{
    LI("LRTTransferSession::OnTickEvent was called\n");
    if (m_queueDataMsg.size()==0) return;
    bool hasData = false;
    for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
    {
        if (m_queueDataMsg.size()>0)
        {
            hasData = true;
            m_packedDataMsg.mutable_msgs(i)->ParseFromString(m_queueDataMsg.front());
            {
                 OSMutexLocker locker(&m_mutexQueueData);
                 m_queueDataMsg.pop();
            }
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

        LI("LRTTransferSession::OnWakeupEvent send data read request to logical server\n");
        this->SendTransferData(tmsg.SerializeAsString());
        for(int i=0;i<MSG_PACKED_ONCE_NUM;++i)
        {
             m_packedDataMsg.mutable_msgs(i)->Clear();
        }
    }
}


// from RTTransfer

void LRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcpNoTimeout::SendTransferData(str.c_str(), (int)str.length());
}

void LRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
#if DEF_PROTO
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    OnTransfer(ack_msg.SerializeAsString());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void LRTTransferSession::OnTypeConn(const std::string& str)
{
#if DEF_PROTO
    pms::ConnMsg c_msg;
    if (!c_msg.ParseFromString(str)) {
        LE("OnTypeConn c_msg ParseFromString error\n");
        return;
    }
    LI("OnTypeConn connmsg--->:\n");

    if ((c_msg.conn_tag() == pms::EConnTag::THI)) {
        // when other connect to ME:
        // send the transfersessionid and ModuleId to other
        pms::TransferMsg t_msg;
        std::string trid;
        GenericSessionId(trid);
        m_transferSessId = trid;

        c_msg.set_tr_module(pms::ETransferModule::MLIVE);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(m_transferSessId);
        //send self Module id to other
        c_msg.set_moduleid(LRTConnManager::Instance().RTLiveId());

        t_msg.set_type(pms::ETransferType::TCONN);
        //this is for transfer
        t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
        t_msg.set_priority(pms::ETransferPriority::PHIGH);
        t_msg.set_content(c_msg.SerializeAsString());

        std::string s = t_msg.SerializeAsString();
        SendTransferData(s.c_str(), (int)s.length());
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLO)) {
        // when ME connector to other:
        // store other's transfersessionid and other's moduleId
        if (c_msg.transferid().length()>0) {
            m_transferSessId = c_msg.transferid();
            {
                LRTConnManager::ModuleInfo* pmi = new LRTConnManager::ModuleInfo();
                if (pmi) {
                    pmi->flag = 1;
                    pmi->othModuleType = c_msg.tr_module();
                    pmi->othModuleId = m_transferSessId;
                    pmi->pModule = this;
                    //bind session and transfer id
                    LRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                    //store which moudle connect to this connector
                    //c_msg._moduleid:store other's module id
                    LI("store other connector moduleid:%s, transfersessionid:%s\n", c_msg.moduleid().c_str(), m_transferSessId.c_str());
                    LRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                } else {
                    LE("new ModuleInfo error!!!\n");
                }
            }

            pms::TransferMsg t_msg;

            c_msg.set_tr_module(pms::ETransferModule::MLIVE);
            c_msg.set_conn_tag(pms::EConnTag::THELLOHI);
            c_msg.set_transferid(m_transferSessId);
            //send self Module id to other
            c_msg.set_moduleid(LRTConnManager::Instance().RTLiveId());

            t_msg.set_type(pms::ETransferType::TCONN);
            //this is for transfer
            t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
            t_msg.set_priority(pms::ETransferPriority::PHIGH);
            t_msg.set_content(c_msg.SerializeAsString());

            std::string s = t_msg.SerializeAsString();
            SendTransferData(s.c_str(), (int)s.length());
        } else {
            LE("Connection id:%s error!!!\n", c_msg.transferid().c_str());
        }
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLOHI)) {
        // when other connect to ME:
        if (m_transferSessId.compare(c_msg.transferid()) == 0) {
            LRTConnManager::ModuleInfo* pmi = new LRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                LRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //store other module id
                LI("store module type:%d, moduleid:%s, transfersessid:%s\n", (int)c_msg.tr_module(), c_msg.moduleid().c_str(), m_transferSessId.c_str());
                LRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
            } else {
                LE("new ModuleInfo error!!!!\n");
            }
        }
    }  else if (c_msg.conn_tag() == pms::EConnTag::TKEEPALIVE) {
        RTTcpNoTimeout::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void LRTTransferSession::OnTypeTrans(const std::string& str)
{
    //LI("%s was called, str:%s\n", __FUNCTION__, str.c_str());
    LRTRTLiveManager::Instance().RecvRequestCounter();
    pms::RelayMsg r_msg;
    r_msg.ParseFromString(str);
    LI("LRTTransferSession::OnTypeTrans srv:cmd:%d\n", r_msg.svr_cmds());
    if (r_msg.svr_cmds() == pms::EServerCmd::CSNDMSG)
    {

    } else if (r_msg.svr_cmds() == pms::EServerCmd::CSYNCSEQN)
    {
        pms::StorageMsg s_msg;
        s_msg.ParseFromString(r_msg.content());
        LI("SYNC SEQN sequence:%lld, userid:%s\n", s_msg.sequence(), s_msg.userid().c_str());
        LRTConnManager::Instance().PushSeqnReq2Queue(r_msg.content());
    } else if (r_msg.svr_cmds() == pms::EServerCmd::CSYNCDATA)
    {
        pms::StorageMsg s_msg;
        s_msg.ParseFromString(r_msg.content());
        LI("SYNC DATA sequence:%lld, userid:%s\n", s_msg.sequence(), s_msg.userid().c_str());
        LRTConnManager::Instance().PushDataReq2Queue(r_msg.content());
    }
}

void LRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called, str:%s\n", __FUNCTION__, str.c_str());
}

void LRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
    pms::PackedStoreMsg store;
    store.ParseFromString(str);
    for(int i=0;i<store.msgs_size();++i)
    {
        if (store.msgs(i).userid().length()==0) break;
        LI("OnTypeDispatch sequence:%lld, userid:%s\n"\
                , store.msgs(i).sequence()\
                , store.msgs(i).userid().c_str());

        pms::TransferMsg t_msg;
        pms::RelayMsg r_msg;
        pms::MsgRep resp;

        // set response
        LI("LRTTransferSession::OnTypeDispatch --->store.svrcmd:%d\n", store.msgs(i).svrcmd());

        resp.set_svr_cmds(store.msgs(i).svrcmd());
        resp.set_mod_type(pms::EModuleType::TLIVE);
        resp.set_rsp_code(0);
        resp.set_rsp_cont(store.msgs(i).SerializeAsString());

        // set relay
        //r_msg.set_svr_cmds(pms::EServerCmd::CSYNCSEQN);
        r_msg.set_svr_cmds(pms::EServerCmd::CSYNCDATA);
        r_msg.set_tr_module(pms::ETransferModule::MLIVE);
        r_msg.set_connector("");
        r_msg.set_content(resp.SerializeAsString());
        pms::ToUser *pto = new pms::ToUser;
        pto->add_users()->assign(store.msgs(i).userid());
        r_msg.set_allocated_touser(pto);

        // set transfer
        t_msg.set_type(pms::ETransferType::TQUEUE);
        t_msg.set_content(r_msg.SerializeAsString());
        std::string response = t_msg.SerializeAsString();
        LRTConnManager::Instance().SendTransferData("", "", response);
    }
}

void LRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeTLogin(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeTLogout(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}


void LRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        LRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

