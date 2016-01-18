//
//  CRTTransferSession.cpp
//  dyncRTConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTTransferSession.h"
#include "RTMessage.h"
#include "atomic.h"
#include "CRTConnectionManager.h"

static unsigned int	g_trans_id = 0;
static unsigned int	g_msg_id = 0;

CRTTransferSession::CRTTransferSession()
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
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
    t_msg._flag = 1;
    t_msg._touser = uid;
    t_msg._connector = CRTConnectionManager::Instance()->ConnectorId();
    t_msg._content = token;
    
    t_trmsg._action = TRANSFERACTION::req;
    t_trmsg._fmodule = TRANSFERMODULE::mconnector;
    t_trmsg._type = TRANSFERTYPE::trans;
    t_trmsg._trans_seq = GenericTransSeq();
    t_trmsg._trans_seq_ack = 0;
    t_trmsg._valid = 1;
    t_trmsg._content = t_msg.ToJson();
    
    const std::string s = t_trmsg.ToJson();
    SendTransferData(s.c_str(), (int)s.length());
    LI("Send user :%s ConnectionLostNotify to meeting module\n", uid.c_str());
}

// from RTTcp
void CRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    {//
        while((m_nBufOffset + nLen) > m_nBufLen)
        {
            m_nBufLen += kRequestBufferSizeInBytes;
            char* ptr = (char *)realloc(m_pBuffer, m_nBufLen);
            if(ptr != NULL)
            {//
                m_pBuffer = ptr;
            }
            else
            {//
                m_nBufLen -= kRequestBufferSizeInBytes;
                continue;
            }
        }
        
        memcpy(m_pBuffer + m_nBufOffset, pData, nLen);
        m_nBufOffset += nLen;
    }
    
    {//
        int parsed = 0;
        parsed = CRTTransfer::ProcessData(m_pBuffer, m_nBufOffset);
        
        if(parsed > 0)
        {
            m_nBufOffset -= parsed;
            if(m_nBufOffset == 0)
            {
                memset(m_pBuffer, 0, m_nBufLen);
            }
            else
            {
                memmove(m_pBuffer, m_pBuffer + parsed, m_nBufOffset);
            }
        }
    }
}

void CRTTransferSession::OnLcsEvent()
{

}

void CRTTransferSession::OnPeerEvent()
{

}

void CRTTransferSession::OnTickEvent()
{

}

// from RTTransfer

void CRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
}

void CRTTransferSession::OnTypeConn(TRANSFERMODULE fmodule, const std::string& str)
{
    CONNMSG c_msg;
    std::string err;
    c_msg.GetMsg(str, err);
    if ((c_msg._tag == CONNTAG::co_msg) && c_msg._msg.compare("hello") == 0) {
        // when other connect to ME:
        // generate TransferSessionId
        // send self ModuleId and TransferSessionId to other
        TRANSFERMSG t_msg;
        std::string trid;
        CRTConnectionManager::Instance()->GenericSessionId(trid);
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
        c_msg._moduleid = CRTConnectionManager::Instance()->ConnectorId();
        
        t_msg._content = c_msg.ToJson();
        std::string s = t_msg.ToJson();
        SendTransferData(s.c_str(), (int)s.length());
    } else if ((c_msg._tag == CONNTAG::co_msgid) && c_msg._msgid.compare("ok") == 0) {
        // when other connect to ME:
        //get other's TransferSessionId and ModuleId
        if (m_transferSessId.compare(c_msg._id)==0) {
            CRTConnectionManager::ModuleInfo* pmi = new CRTConnectionManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                //store other's module type
                pmi->othModuleType = fmodule;
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                CRTConnectionManager::Instance()->AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //c_msg._moduleid: store other's module id
                CRTConnectionManager::Instance()->AddTypeModuleSession(fmodule, c_msg._moduleid, m_transferSessId);
                LI("store the other's module id:%s, transfersessid:%s\n", c_msg._moduleid.c_str(), c_msg._id.c_str());
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
        LE("%s parse error:%s\n", __FUNCTION__, err.c_str());
        return;
    }
    {
        //find connection
    }
    {
        //send response
        std::string msg(dmsg._content);
        std::string id(dmsg._touser);
        LI("OnTypeDispatch id:%s, msg:%s\n", id.c_str(), msg.c_str());
        m_dispatchConnection.DispatchMsg(id, msg);
    }
}

void CRTTransferSession::OnTypePush(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        LI("RTTransferSession::ConnectionDisconnected m_transferSessId:%s\n", m_transferSessId.c_str());
        CRTConnectionManager::Instance()->TransferSessionLostNotify(m_transferSessId);
    } else {
        LE("RTConnection::ConnectionDisconnected m_transferSessId.length is 0\n");
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

void CRTTransferSession::GenericMsgId(std::string& strId)
{
    char buf[32] = {0};
    int id_ = (UInt32)atomic_add(&g_msg_id, 1);
    sprintf(buf, "msgqueue_%06d", id_);
    strId = buf;
}

int CRTTransferSession::GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
}

void CRTTransferSession::EstablishAck()
{

}

void CRTTransferSession::OnEstablishConn()
{

}

void CRTTransferSession::OnEstablishAck()
{

}
