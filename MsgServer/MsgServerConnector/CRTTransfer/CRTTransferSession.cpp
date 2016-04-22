//
//  CRTTransferSession.cpp
//  dyncRTConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTTransferSession.h"
#include "RTMessage.h"
#include "CRTConnManager.h"
#include "RTUtils.hpp"

CRTTransferSession::CRTTransferSession()
: RTJSBuffer()
, RTTransfer()
, m_transferSessId("")
{
    AddObserver(this);
}

CRTTransferSession::~CRTTransferSession()
{
    DelObserver(this);
    Unit();
}

void CRTTransferSession::Init()
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

void CRTTransferSession::Unit()
{
    Disconn();
}

bool CRTTransferSession::Connect(const std::string addr, int port)
{
    if (addr.empty() || port < 2048) {
        LE("%s invalid params addr:%s, port:%d\n", __FUNCTION__, addr.c_str(), port);
        return false;
    }
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(addr.c_str()), port);
    if (err == OS_NoErr || err == EISCONN) {
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

void CRTTransferSession::Disconn()
{
    //GetSocket()->Cleanup();
}

void CRTTransferSession::TestConnection()
{

}

void CRTTransferSession::EstablishConnection()
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
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

void CRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void CRTTransferSession::ConnectionLostNotify(const std::string& uid, const std::string& token)
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::RelayMsg r_msg;
    pms::ToUser *pto = new pms::ToUser;
    pto->add_users(uid);

    r_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
    r_msg.set_connector(CRTConnManager::Instance().ConnectorId());
    r_msg.set_content(token);
    r_msg.set_allocated_touser(pto);

    t_msg.set_type(pms::ETransferType::TLOGOUT);
    t_msg.set_content(r_msg.SerializeAsString());

    const std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTTransferSession::ConnectionConnNotify(const std::string& uid, const std::string& token)
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::RelayMsg r_msg;
    pms::ToUser *pto = new pms::ToUser;
    pto->add_users(uid);

    r_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
    r_msg.set_connector(CRTConnManager::Instance().ConnectorId());
    r_msg.set_content(token);
    r_msg.set_allocated_touser(pto);


    t_msg.set_type(pms::ETransferType::TLOGIN);
    t_msg.set_content(r_msg.SerializeAsString());

    const std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTTransferSession::TransferMsg(const std::string& msg)
{
#if DEF_PROTO
    pms::TransferMsg t_msg;
    pms::RelayMsg r_msg;

    r_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
    r_msg.set_connector(CRTConnManager::Instance().ConnectorId());
    r_msg.set_content(msg);

    t_msg.set_type(pms::ETransferType::TTRANS);
    t_msg.set_content(r_msg.SerializeAsString());

    const std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
#else
    LE("not define DEF_PROTO\n");
#endif
}

// from RTTcp
void CRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void CRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
    RTTransfer::DoProcessData(message, nLen);
}

// from RTTransfer

void CRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
}

void CRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
#if DEF_PROTO
    LI("CRTTransferSession::OnMsgAck...\n");
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    const std::string s = ack_msg.SerializeAsString();
    OnTransfer(s);
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTTransferSession::OnTypeConn(const std::string& str)
{
#if DEF_PROTO
    LI("CRTTransferSession::OnTypeConn was enter\n");
    pms::ConnMsg c_msg;
    if (!c_msg.ParseFromString(str)) {
        LE("OnTypeConn c_msg.ParseFromString error\n");
    }
    LI("CRTTransferSession::OnTypeConn--->\n");
    c_msg.PrintDebugString();

    if ((c_msg.conn_tag() == pms::EConnTag::THI)) {
        // when other connect to ME:
        // generate TransferSessionId
        // send self ModuleId and TransferSessionId to other
        pms::TransferMsg t_msg;
        std::string trid;
        GenericSessionId(trid);
        m_transferSessId = trid;

        //this is for transfer
        t_msg.set_type(pms::ETransferType::TCONN);
        t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
        t_msg.set_priority(pms::ETransferPriority::PHIGH);

        c_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(trid);
        //send self connector to other
        c_msg.set_moduleid(CRTConnManager::Instance().ConnectorId());

        t_msg.set_content(c_msg.SerializeAsString());
        std::string s = t_msg.SerializeAsString();
        LI("CRTTransferSession::OnTypeConn SendTransferData...\n");
        SendTransferData(s.c_str(), (int)s.length());
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLOHI)) {
        // when other connect to ME:
        //get other's TransferSessionId and ModuleId
        if (m_transferSessId.compare(c_msg.transferid())==0) {
            CRTConnManager::ModuleInfo* pmi = new CRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                //store other's module type
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                CRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //c_msg._moduleid: store other's module id
                CRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                LI("store the other's module :%d, id:%s, transfersessid:%s\n", (int)c_msg.tr_module(),  c_msg.moduleid().c_str(), c_msg.transferid().c_str());
            } else {
                LE("new ModuleInfo error!!!\n");
            }
        }
    } else if (c_msg.conn_tag() == pms::EConnTag::TKEEPALIVE) {
        this->RTTcp::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeDispatch(const std::string& str)
{
#if DEF_PROTO
    pms::RelayMsg dmsg;
    if (!dmsg.ParseFromString(str)) {
        LE("OnTypeDispatch dmsg.ParseFromString error\n");
    }
    pms::ToUser to = dmsg.touser();
    LI("CRTTransferSession::OnTypeDispatch--->touser:\n");
    to.PrintDebugString();
#if 0
    pms::MsgRep resp;
    resp.ParseFromString(dmsg.content());
    LI("CRTTransferSession::OnTypeDispatch--->resp:\n");
    resp.PrintDebugString();
    pms::MeetMsg meet;
    meet.ParseFromString(resp.rsp_cont());
    LI("CRTTransferSession::OnTypeDispatch--->meet:\n");
    meet.PrintDebugString();
#endif

    {
        for(int i = 0;i < to.users_size(); ++i) {
            m_dispatchConnection.DispatchMsg(to.users(i), dmsg.content());
        }
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeTLogin(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeTLogout(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        LI("CRTTransferSession::ConnectionDisconnected m_transferSessId:%s\n", m_transferSessId.c_str());
        CRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
