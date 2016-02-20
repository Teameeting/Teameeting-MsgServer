//
//  DRTTransferSession.cpp
//  dyncRTMsgQueue
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include "DRTTransferSession.h"
#include "RTMessage.h"
#include "DRTConnectionManager.h"
#include "RTUtils.hpp"

#define TIMEOUT_TS (60*1000)


DRTTransferSession::DRTTransferSession()
: RTJSBuffer()
, RTTransfer()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
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
}

bool DRTTransferSession::Connect(const std::string addr, int port)
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
    TRANSFERMSG t_msg;
    CONNMSG c_msg;
    t_msg._action = TRANSFERACTION::req;
    t_msg._fmodule = TRANSFERMODULE::mmsgqueue;
    t_msg._type = TRANSFERTYPE::conn;
    t_msg._trans_seq = GenericTransSeq();
    t_msg._trans_seq_ack = 0;
    t_msg._valid = 1;

    c_msg._tag = CONNTAG::co_keepalive;
    c_msg._msg = "1";
    c_msg._id = "";
    c_msg._msgid = "";
    c_msg._moduleid = "";
    t_msg._content = c_msg.ToJson();

    std::string s = t_msg.ToJson();
    SendTransferData(s.c_str(), (int)s.length());
}

void DRTTransferSession::TestConnection()
{

}

void DRTTransferSession::EstablishConnection()
{
    TRANSFERMSG t_msg;
    CONNMSG c_msg;
    t_msg._action = TRANSFERACTION::req;
    t_msg._fmodule = TRANSFERMODULE::mmsgqueue;
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

void DRTTransferSession::OnMsgAck(TRANSFERMSG& tmsg)
{
    TRANSFERMSG ack_msg;
    if (tmsg._action == TRANSFERACTION::req) {
        ack_msg._action = TRANSFERACTION::req_ack;
    } else {
        ack_msg._action = TRANSFERACTION::resp_ack;
    }
    ack_msg._fmodule = TRANSFERMODULE::mmsgqueue;
    ack_msg._type   = tmsg._type;
    ack_msg._trans_seq = tmsg._trans_seq;
    ack_msg._trans_seq_ack = tmsg._trans_seq + 1;
    ack_msg._valid = tmsg._valid;
    ack_msg._content = "";
    const std::string s = ack_msg.ToJson();
    OnTransfer(s);
}

void DRTTransferSession::OnTypeConn(TRANSFERMODULE fmodule, const std::string& str)
{
    CONNMSG c_msg;
    std::string err;
    c_msg.GetMsg(str, err);
    if (err.length() > 0) {
        //connid error
        LE("%s parsed err:%s\n", __FUNCTION__, err.c_str());
        return;
    }
    if ((c_msg._tag == CONNTAG::co_msg) && c_msg._msg.compare("hello") == 0) {
        // when other connect to ME:
        // send the transfersessionid and MsgQueueId to other
        TRANSFERMSG t_msg;
        std::string trid;
        GenericSessionId(trid);
        m_transferSessId = trid;

        t_msg._action = TRANSFERACTION::req;
        //this is for transfer
        t_msg._fmodule = TRANSFERMODULE::mmsgqueue;
        t_msg._type = TRANSFERTYPE::conn;
        t_msg._trans_seq = GenericTransSeq();
        t_msg._trans_seq_ack = 0;
        t_msg._valid = 1;

        c_msg._tag = CONNTAG::co_id;
        c_msg._id = m_transferSessId;
        //send self MsgQueue id to other
        c_msg._moduleid = DRTConnectionManager::Instance()->MsgQueueId();

        t_msg._content = c_msg.ToJson();

        std::string s = t_msg.ToJson();
        SendTransferData(s.c_str(), (int)s.length());
    } else if ((c_msg._tag == CONNTAG::co_id) && c_msg._msg.compare("hello") == 0) {
        // when ME connector to other:
        // store other's transfersessionid and other's moduleId
        if (c_msg._id.length()>0) {
            m_transferSessId = c_msg._id;
            {
                DRTConnectionManager::ModuleInfo* pmi = new DRTConnectionManager::ModuleInfo();
                if (pmi) {
                    pmi->flag = 1;
                    pmi->othModuleType = fmodule;
                    pmi->othModuleId = m_transferSessId;
                    pmi->pModule = this;
                    //bind session and transfer id
                    DRTConnectionManager::Instance()->AddModuleInfo(pmi, m_transferSessId);
                    //store which moudle connect to this connector
                    //c_msg._moduleid:store other's module id
                    LI("store other connector moduleid:%s, transfersessionid:%s\n", c_msg._moduleid.c_str(), m_transferSessId.c_str());
                    DRTConnectionManager::Instance()->AddTypeModuleSession(fmodule, c_msg._moduleid, m_transferSessId);
                } else {
                    LE("new ModuleInfo error!!!\n");
                }
            }

            TRANSFERMSG t_msg;

            t_msg._action = TRANSFERACTION::req;
            t_msg._fmodule = TRANSFERMODULE::mmsgqueue;
            t_msg._type = TRANSFERTYPE::conn;
            t_msg._trans_seq = GenericTransSeq();
            t_msg._trans_seq_ack = 0;
            t_msg._valid = 1;

            c_msg._tag = CONNTAG::co_msgid;
            c_msg._id = m_transferSessId;
            c_msg._msgid = "ok";
            //send self MsgQueue id to other
            c_msg._moduleid = DRTConnectionManager::Instance()->MsgQueueId();

            t_msg._content = c_msg.ToJson();

            std::string s = t_msg.ToJson();
            SendTransferData(s.c_str(), (int)s.length());
        } else {
            LE("Connection id:%s error!!!\n", c_msg._id.c_str());
        }
    } else if ((c_msg._tag == CONNTAG::co_msgid) && c_msg._msgid.compare("ok") == 0) {
        // when other connect to ME:
        if (m_transferSessId.compare(c_msg._id) == 0) {
            DRTConnectionManager::ModuleInfo* pmi = new DRTConnectionManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = fmodule;
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                DRTConnectionManager::Instance()->AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //store other module id
                LI("store moduleid:%s, transfersessid:%s\n", c_msg._moduleid.c_str(), m_transferSessId.c_str());
                DRTConnectionManager::Instance()->AddTypeModuleSession(fmodule, c_msg._moduleid, m_transferSessId);
            } else {
                LE("new ModuleInfo error!!!!\n");
            }
        }

    }  else if (c_msg._tag == CONNTAG::co_keepalive) {
        RTTcp::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
}

void DRTTransferSession::OnTypeTrans(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called, str:%s\n", __FUNCTION__, str.c_str());
}

void DRTTransferSession::OnTypeQueue(TRANSFERMODULE fmodule, const std::string& str)
{
    TOJSONUSER auser;//all user
    QUEUEMSG qmsg;
    {
        //get auser
        std::string err;
        qmsg.GetMsg(str, err);
        if (err.length()>0) {
            LE("%s QUEUEMSG err:%s\n", __FUNCTION__, err.c_str());
            return;
        }
        auser.GetMsg(qmsg._touser, err);
        if (err.length()>0) {
            LE("%s TOJSONUSER err:%s\n", __FUNCTION__, err.c_str());
            return;
        }
    }
    bool needDispatch = false;
    bool needPush = false;
    TOJSONUSER duser;//dispatcher
    TOJSONUSER puser;//pusher
    {
        //check user online or offline
        std::list<std::string>::iterator it = auser._us.begin();
        for (; it!=auser._us.end(); it++) {
            if (DRTConnectionManager::Instance()->IsMemberInOnline((*it))) {
                duser._us.push_back((*it));
                needDispatch = true;
            } else {
                puser._us.push_back((*it));
                needPush = true;
            }
        }
    }
    {
        //if online, push to online msgqueue
        if (needDispatch) {
            DISPATCHMSG dmsg;
            dmsg._flag = 0;
            dmsg._touser = duser.ToJson();
            dmsg._connector = qmsg._connector;//which connector comes from
            dmsg._content = qmsg._content;
            std::string sd = dmsg.ToJson();
            LI("OnTypeQueue dmsg._touser:%s, dmsg._connector.c_str():%s\n", dmsg._touser.c_str(), dmsg._connector.c_str());
            m_msgDispatch.SendData(sd.c_str(), (int)sd.length());
        }
    }
    {
        //if offline, push to offline msgqueue
        if (needPush) {
            PUSHMSG pmsg;
            pmsg._flag = 0;
            pmsg._touser = puser.ToJson();
            pmsg._connector = qmsg._connector;//which connector comes from
            pmsg._content = qmsg._content;
            
            std::string sp = pmsg.ToJson();
            LI("OnTypeQueue pmsg._touser:%s, pmsg._connector.c_str():%s\n", pmsg._touser.c_str(), pmsg._connector.c_str());
            m_msgDispatch.PushData(sp.c_str(), (int)sp.length());
        }
    }
}

void DRTTransferSession::OnTypeDispatch(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void DRTTransferSession::OnTypePush(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void DRTTransferSession::OnTypeTLogin(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
    TRANSMSG t_msg;
    std::string err;
    t_msg.GetMsg(str, err);
    if (err.length() > 0) {
        LE("%s TRANSMSG error:%s\n", __FUNCTION__, err.c_str());
        Assert(false);
        return;
    }
    LI("TLogin user:%s, token:%s, connector:%s\n", t_msg._touser.c_str(), t_msg._content.c_str(), t_msg._connector.c_str());
    DRTConnectionManager::Instance()->OnTLogin(t_msg._touser, t_msg._content);
}

void DRTTransferSession::OnTypeTLogout(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
    TRANSMSG t_msg;
    std::string err;
    t_msg.GetMsg(str, err);
    if (err.length() > 0) {
        LE("%s TRANSMSG error:%s\n", __FUNCTION__, err.c_str());
        Assert(false);
        return;
    }
    LI("TLogout user:%s, token:%s, connector:%s\n", t_msg._touser.c_str(), t_msg._content.c_str(), t_msg._connector.c_str());
    DRTConnectionManager::Instance()->OnTLogout(t_msg._touser, t_msg._content);
}


void DRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        DRTConnectionManager::Instance()->TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

