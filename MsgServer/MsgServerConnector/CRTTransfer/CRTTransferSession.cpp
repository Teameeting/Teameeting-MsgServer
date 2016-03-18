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
    GetSocket()->Cleanup();
}

void CRTTransferSession::TestConnection()
{

}

void CRTTransferSession::EstablishConnection()
{
    TRANSFERMSG t_msg;
    CONNMSG c_msg;
    t_msg._action = TRANSFERACTION::req;
    t_msg._fmodule = TRANSFERMODULE::mconnector;
    t_msg._type = TRANSFERTYPE::conn;
    t_msg._trans_seq = GenericTransSeq();
    t_msg._trans_seq_ack = 0;
    t_msg._valid = 1;

    c_msg._tag = CONNTAG::co_msg;
    c_msg._msg = "hello";
    c_msg._id = "";
    c_msg._msgid = "";
    c_msg._moduleid = "";

    t_msg._content = c_msg.ToJson();

    std::string s = t_msg.ToJson();
    SendTransferData(s.c_str(), (int)s.length());
}

void CRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void CRTTransferSession::ConnectionLostNotify(const std::string& uid, const std::string& token)
{
    TRANSFERMSG t_trmsg;
    TRANSMSG t_msg;
    t_msg._flag = 0;
    t_msg._touser = uid;
    t_msg._connector = CRTConnManager::Instance()->ConnectorId();
    t_msg._content = token;

    t_trmsg._action = TRANSFERACTION::req;
    t_trmsg._fmodule = TRANSFERMODULE::mconnector;
    t_trmsg._type = TRANSFERTYPE::tlogout;
    t_trmsg._trans_seq = GenericTransSeq();
    t_trmsg._trans_seq_ack = 0;
    t_trmsg._valid = 1;
    t_trmsg._content = t_msg.ToJson();

    const std::string s = t_trmsg.ToJson();
    SendTransferData(s.c_str(), (int)s.length());
}

void CRTTransferSession::ConnectionConnNotify(const std::string& uid, const std::string& token)
{
    TRANSFERMSG t_trmsg;
    TRANSMSG t_msg;
    t_msg._flag = 0;
    t_msg._touser = uid;
    t_msg._connector = CRTConnManager::Instance()->ConnectorId();
    t_msg._content = token;

    t_trmsg._action = TRANSFERACTION::req;
    t_trmsg._fmodule = TRANSFERMODULE::mconnector;
    t_trmsg._type = TRANSFERTYPE::tlogin;
    t_trmsg._trans_seq = GenericTransSeq();
    t_trmsg._trans_seq_ack = 0;
    t_trmsg._valid = 1;
    t_trmsg._content = t_msg.ToJson();

    const std::string s = t_trmsg.ToJson();
    SendTransferData(s.c_str(), (int)s.length());
}

void CRTTransferSession::TransferMsg(const std::string& msg)
{
    TRANSFERMSG t_trmsg;
    TRANSMSG t_msg;
    t_msg._flag = 0;
    t_msg._touser = "";
    t_msg._connector = CRTConnManager::Instance()->ConnectorId();
    t_msg._content = msg;
    
    t_trmsg._action = TRANSFERACTION::req;
    t_trmsg._fmodule = TRANSFERMODULE::mconnector;
    t_trmsg._type = TRANSFERTYPE::trans;
    t_trmsg._trans_seq = GenericTransSeq();
    t_trmsg._trans_seq_ack = 0;
    t_trmsg._valid = 1;
    t_trmsg._content = t_msg.ToJson();
    
    const std::string s = t_trmsg.ToJson();
    SendTransferData(s.c_str(), (int)s.length());
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

void CRTTransferSession::OnMsgAck(TRANSFERMSG& tmsg)
{
    TRANSFERMSG ack_msg;
    if (tmsg._action == TRANSFERACTION::req) {
        ack_msg._action = TRANSFERACTION::req_ack;
    } else {
        ack_msg._action = TRANSFERACTION::resp_ack;
    }
    ack_msg._fmodule = TRANSFERMODULE::mconnector;
    ack_msg._type   = tmsg._type;
    ack_msg._trans_seq = tmsg._trans_seq;
    ack_msg._trans_seq_ack = tmsg._trans_seq + 1;
    ack_msg._valid = tmsg._valid;
    ack_msg._content = "";
    const std::string s = ack_msg.ToJson();
    OnTransfer(s);
}

void CRTTransferSession::OnTypeConn(TRANSFERMODULE fmodule, const std::string& str)
{
    CONNMSG c_msg;
    std::string err;
    c_msg.GetMsg(str, err);
    if (err.length()>0) {
        LE("CRTTransferSession::OnTypeConn GetMsg err:%s\n", err.c_str());
        return;
    }
    if ((c_msg._tag == CONNTAG::co_msg) && c_msg._msg.compare("hello") == 0) {
        // when other connect to ME:
        // generate TransferSessionId
        // send self ModuleId and TransferSessionId to other
        TRANSFERMSG t_msg;
        std::string trid;
        GenericSessionId(trid);
        m_transferSessId = trid;

        t_msg._action = TRANSFERACTION::req;
        //this is for transfer
        t_msg._fmodule = TRANSFERMODULE::mconnector;
        t_msg._type = TRANSFERTYPE::conn;
        t_msg._trans_seq = GenericTransSeq();
        t_msg._trans_seq_ack = 0;
        t_msg._valid = 1;

        c_msg._tag = CONNTAG::co_id;
        c_msg._id = trid;
        //send self connector to other
        c_msg._moduleid = CRTConnManager::Instance()->ConnectorId();

        t_msg._content = c_msg.ToJson();
        std::string s = t_msg.ToJson();
        SendTransferData(s.c_str(), (int)s.length());
    } else if ((c_msg._tag == CONNTAG::co_msgid) && c_msg._msgid.compare("ok") == 0) {
        // when other connect to ME:
        //get other's TransferSessionId and ModuleId
        if (m_transferSessId.compare(c_msg._id)==0) {
            CRTConnManager::ModuleInfo* pmi = new CRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                //store other's module type
                pmi->othModuleType = fmodule;
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                CRTConnManager::Instance()->AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //c_msg._moduleid: store other's module id
                CRTConnManager::Instance()->AddTypeModuleSession(fmodule, c_msg._moduleid, m_transferSessId);
                LI("store the other's module fmodule:%d, id:%s, transfersessid:%s\n", (int)fmodule,  c_msg._moduleid.c_str(), c_msg._id.c_str());
            } else {
                LE("new ModuleInfo error!!!\n");
            }
        }

    } else if (c_msg._tag == CONNTAG::co_keepalive) {
        this->RTTcp::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
}

void CRTTransferSession::OnTypeTrans(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeQueue(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeDispatch(TRANSFERMODULE fmodule, const std::string& str)
{
    DISPATCHMSG dmsg;
    std::string err;
    dmsg.GetMsg(str, err);
    if (err.length()>0) {
        LE("%s parse DIAPTCHMSG error:%s\n", __FUNCTION__, err.c_str());
        return;
    }
    
    {
        //send response
        TOJSONUSER touser;
        touser.GetMsg(dmsg._touser, err);
        if (err.length()>0) {
            LE("%s parse TOJSONUSER error:%s\n", __FUNCTION__, err.c_str());
            return;
        }
        for (std::list<std::string>::iterator it=touser._us.begin(); it!=touser._us.end(); it++) {
            m_dispatchConnection.DispatchMsg((*it), dmsg._content);
        }
    }
}

void CRTTransferSession::OnTypePush(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeTLogin(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeTLogout(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        LI("RTTransferSession::ConnectionDisconnected m_transferSessId:%s\n", m_transferSessId.c_str());
        CRTConnManager::Instance()->TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
