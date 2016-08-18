//
//  DRTTransferSession.cpp
//  dyncRTMsgQueue
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "DRTTransferSession.h"
#include "DRTConnManager.h"
#include "RTUtils.hpp"

#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"
#include "MsgServer/proto/storage_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/storage_msg.pb.h"
#include "MsgServer/proto/entity_msg.pb.h"
#include "MsgServer/proto/entity_msg_type.pb.h"

#define TIMEOUT_TS (60*1000)


DRTTransferSession::DRTTransferSession()
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

DRTTransferSession::~DRTTransferSession()
{
    DelObserver(this);
    Unit();
}

void DRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(96L * 1024L);

    socket->SetTask(this);
    this->SetTimer(120*1000);
}

void DRTTransferSession::Unit()
{
    Disconn();
    m_connectingStatus = 0;
}

bool DRTTransferSession::Connect(const std::string addr, int port)
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

bool DRTTransferSession::Connect()
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

void DRTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool DRTTransferSession::RefreshTime()
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

void DRTTransferSession::KeepAlive()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MMSGQUEUE);
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

void DRTTransferSession::TestConnection()
{

}

void DRTTransferSession::EstablishConnection()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MMSGQUEUE);
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

void DRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

// from RTTcp
void DRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void DRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
    RTTransfer::DoProcessData(message, nLen);
}


// from RTTransfer

void DRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
}

void DRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
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

void DRTTransferSession::OnTypeConn(const std::string& str)
{
#if DEF_PROTO
    pms::ConnMsg c_msg;
    if (!c_msg.ParseFromString(str)) {
        LE("OnTypeConn c_msg ParseFromString error\n");
        return;
    }
    LI("OnTypeConn connmsg--->:\n");
    //c_msg.PrintDebugString();

    if ((c_msg.conn_tag() == pms::EConnTag::THI)) {
        // when other connect to ME:
        // send the transfersessionid and MsgQueueId to other
        pms::TransferMsg t_msg;
        std::string trid;
        GenericSessionId(trid);
        m_transferSessId = trid;

        c_msg.set_tr_module(pms::ETransferModule::MMSGQUEUE);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(m_transferSessId);
        //send self MsgQueue id to other
        c_msg.set_moduleid(DRTConnManager::Instance().MsgQueueId());

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
                DRTConnManager::ModuleInfo* pmi = new DRTConnManager::ModuleInfo();
                if (pmi) {
                    pmi->flag = 1;
                    pmi->othModuleType = c_msg.tr_module();
                    pmi->othModuleId = m_transferSessId;
                    pmi->pModule = this;
                    //bind session and transfer id
                    DRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                    //store which moudle connect to this connector
                    //c_msg._moduleid:store other's module id
                    LI("store other connector moduleid:%s, transfersessionid:%s\n", c_msg.moduleid().c_str(), m_transferSessId.c_str());
                    DRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                } else {
                    LE("new ModuleInfo error!!!\n");
                }
            }

            pms::TransferMsg t_msg;

            c_msg.set_tr_module(pms::ETransferModule::MMSGQUEUE);
            c_msg.set_conn_tag(pms::EConnTag::THELLOHI);
            c_msg.set_transferid(m_transferSessId);
            //send self MsgQueue id to other
            c_msg.set_moduleid(DRTConnManager::Instance().MsgQueueId());

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
            DRTConnManager::ModuleInfo* pmi = new DRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                DRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //store other module id
                LI("store module type:%d, moduleid:%s, transfersessid:%s\n", (int)c_msg.tr_module(), c_msg.moduleid().c_str(), m_transferSessId.c_str());
                DRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
            } else {
                LE("new ModuleInfo error!!!!\n");
            }
        }
    }  else if (c_msg.conn_tag() == pms::EConnTag::TKEEPALIVE) {
        RTTcp::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void DRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called, str:%s\n", __FUNCTION__, str.c_str());
}

void DRTTransferSession::OnTypeQueue(const std::string& str)
{
#if DEF_PROTO
    pms::RelayMsg rmsg;
    if (!rmsg.ParseFromString(str)) {
        LE("r_msg.ParseFromString error\n");
        return;
    }
    LI("DRTTransferSession::OnTypeQueue was called\n");
    //get all user
    pms::ToUser auser = rmsg.touser();
    bool needDispatch = false;
    bool needPush = false;
    pms::RelayMsg pmsg;
    pms::ToUser *pallduser = pmsg.mutable_touser();
    DRTConnManager::UserConnectorMaps connUserId;
    LI("DRTTransferSession::OnTypeQueue handle_cmd:%s, handle_mtype:%s, handle_data:%s\n", rmsg.handle_cmd().c_str(), rmsg.handle_mtype().c_str(), rmsg.handle_data().c_str());
#if 0
    {
        //check user online or offline
        for (int i=0; i<auser.users_size(); ++i) {
            //if (DRTConnManager::Instance().IsMemberInOnline((auser.users(i)))) {
                std::string cid("");
                DRTConnManager::Instance().GetUserConnectorId((auser.users(i)), cid);
                connUserId.insert(make_pair(cid, (auser.users(i))));
                pallduser->add_users(auser.users(i));
                needDispatch = true;
            //} else {
                ////add all the needed push user;
            //    LI("DRTTransferSession::OnTypeQueue user:%s need push\n", auser.users(i).c_str());
            //    needPush = true;
            //}
        }
    }
    {
        //if online, push to online msgqueue
        if (needDispatch) {
            unsigned i=0, j=(unsigned)connUserId.bucket_count();
            for (; i<j; ++i) {
                if (connUserId.begin(i)==connUserId.end(i)) {
                    continue;
                }
                std::string sess = connUserId.begin(i)->first;
                pms::RelayMsg dmsg;
                pms::ToUser *pduser = dmsg.mutable_touser();
                for (auto& x:connUserId) {
                    pduser->add_users(x.second);
                }
                dmsg.set_svr_cmds(rmsg.svr_cmds());
                dmsg.set_tr_module(rmsg.tr_module());
                dmsg.set_content(rmsg.content());
                dmsg.set_connector(connUserId.begin(i)->first);//which connector comes from

                std::string sd = dmsg.SerializeAsString();
                m_msgDispatch.SendData(sd.c_str(), (int)sd.length());

                LI("OnTypeQueue dispatch msg--->:\n");
                //dmsg.PrintDebugString();
            }
        }
    }
#else
    std::string sd = rmsg.SerializeAsString();
    m_msgDispatch.SendData(sd.c_str(), (int)sd.length());
#endif
    {
        //if offline, push to offline msgqueue
        if (needPush) {
            pmsg.set_svr_cmds(rmsg.svr_cmds());
            pmsg.set_tr_module(rmsg.tr_module());
            pmsg.set_content(rmsg.content());
            pmsg.set_connector(rmsg.connector());//which connector comes from

            std::string sp = pmsg.SerializeAsString();
            m_msgDispatch.PushData(sp.c_str(), (int)sp.length());

            LI("OnTypeQueue push msg--->:\n");
            //pmsg.PrintDebugString();
        }
    }
#else
    LI("not define DEF_PROTO\n");
#endif
}

void DRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void DRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void DRTTransferSession::OnTypeTLogin(const std::string& str)
{
#if DEF_PROTO
    pms::RelayMsg rmsg;
    if (!rmsg.ParseFromString(str)) {
        LE("OnTypeLogin rmsg.ParseFromString error\n");
    }
    Assert(rmsg.touser.users_size()==1);
    DRTConnManager::Instance().OnTLogin(rmsg.touser().users(0), rmsg.content(), rmsg.connector());
#else
    LI("not define DEF_PROTO\n");
#endif
}

void DRTTransferSession::OnTypeTLogout(const std::string& str)
{
#if DEF_PROTO
    pms::RelayMsg rmsg;
    if (!rmsg.ParseFromString(str)) {
        LE("OnTypeLogout rmsg.ParseFromString error\n");
    }
    Assert(rmsg.touser.users_size()==1);
    DRTConnManager::Instance().OnTLogout(rmsg.touser().users(0), rmsg.content(), rmsg.connector());
#else
    LI("not define DEF_PROTO\n");
#endif
}


void DRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        DRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

