//
//  DRTTransferSession.cpp
//  dyncRTMsgQueue
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "DRTTransferSession.h"
#include "RTMessage.h"
#include "atomic.h"
#include "DRTConnectionManager.h"
#include <list>

#define TIMEOUT_TS (60*1000)

static unsigned int	g_trans_id = 0;
static unsigned int	g_msg_id = 0;

DRTTransferSession::DRTTransferSession()
: RTJSBuffer()
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
    DRTTransfer::DoProcessData(message, nLen);
}

void DRTTransferSession::OnLcsEvent()
{

}

void DRTTransferSession::OnPeerEvent()
{

}

void DRTTransferSession::OnTickEvent()
{

}

// from RTTransfer

void DRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
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
        DRTConnectionManager::Instance()->GenericSessionId(trid);
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
    TOJSONUSER user;
    QUEUEMSG qmsg;
    DISPATCHMSG dmsg;
    TRANSFERMSG trmsg;
    {
        //get user
        std::string err;
        qmsg.GetMsg(str, err);
        if (err.length()>0) {
            LE("%s QUEUEMSG err:%s\n", __FUNCTION__, err.c_str());
            return;
        }
        user.GetMsg(qmsg._touser, err);
        if (err.length()>0) {
            LE("%s TOJSONUSER err:%s\n", __FUNCTION__, err.c_str());
            return;
        }
    }
    {
        //check user online or offline
    }
    {
        //if offline, push to offline msgqueue
    }
    {
        //if online, push to online msgqueue
    }

    std::list<std::string>::iterator it = user._us.begin();
    for (; it!=user._us.end(); it++) {
        dmsg._flag = 0;
        dmsg._touser = (*it);
        dmsg._connector = qmsg._connector;//which connector comes frome
        dmsg._content = qmsg._content;

        trmsg._action = TRANSFERACTION::req;
        trmsg._fmodule = TRANSFERMODULE::mmsgqueue;
        trmsg._type = TRANSFERTYPE::dispatch;
        trmsg._trans_seq = GenericTransSeq();
        trmsg._trans_seq_ack = 0;
        trmsg._valid = 1;
        std::string sd = dmsg.ToJson();
        trmsg._content = sd;

        LI("OnTypeQueue dmsg._touser:%s, qmsg._connector.c_str():%s\n", dmsg._touser.c_str(), qmsg._connector.c_str());
        //here we should know this msg send to whom
        //find connector and dispatch to it
        std::string st = trmsg.ToJson();
        //connector moduleId
        DRTConnectionManager::ModuleInfo* pmi = DRTConnectionManager::Instance()->findConnectorInfoById(dmsg._touser, qmsg._connector);
        if (pmi && pmi->pModule) {
            pmi->pModule->SendTransferData(st.c_str(), (int)st.length());
        } else {
            LE("mi.pModule is NULL\n");
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

void DRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        DRTConnectionManager::Instance()->TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

void DRTTransferSession::GenericMsgId(std::string& strId)
{
    char buf[32] = {0};
    int id_ = (UInt32)atomic_add(&g_msg_id, 1);
    sprintf(buf, "msgqueue_%06d", id_);
    strId = buf;
}

int DRTTransferSession::GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
}

void DRTTransferSession::EstablishAck()
{

}

void DRTTransferSession::OnEstablishConn()
{

}

void DRTTransferSession::OnEstablishAck()
{

}
