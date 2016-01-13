//
//  RTTransferSession.cpp
//  dyncRTMeeting
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTTransferSession.h"
#include "RTMessage.h"
#include "atomic.h"
#include "RTRoomManager.h"
#include "RTConnectionManager.h"
#include "RTHttpSvrConn.h"

#define TIMEOUT_TS (60*1000)
static unsigned int	g_trans_id = 0;
static unsigned int	g_msg_id = 0;

RTTransferSession::RTTransferSession()
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
{
    AddObserver(this);
}

RTTransferSession::~RTTransferSession()
{
    DelObserver(this);
    Unit();
    if (m_pBuffer) {
        free(m_pBuffer);
        m_pBuffer = NULL;
    }
}

void RTTransferSession::Init()
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

void RTTransferSession::Unit()
{
    Disconn();
}

bool RTTransferSession::Connect(const std::string addr, int port)
{
    if (addr.empty() || port < 8192) {
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

void RTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool RTTransferSession::RefreshTime()
{
    UInt64 now = OS::Milliseconds();
    //LE("RTTransferSession::RefreshTime now:%llu, lastTime:%llu\n", now, m_lastUpdateTime);
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        RTTcp::UpdateTimer();
        return true;
    } else {
        return false;
    }
}

void RTTransferSession::KeepAlive()
{
    TRANSFERMSG t_msg;
    CONNMSG c_msg;
    t_msg._action = TRANSFERACTION::req;
    t_msg._fmodule = TRANSFERMODULE::mmeeting;
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


void RTTransferSession::TestConnection()
{
    
}

void RTTransferSession::EstablishConnection()
{
    TRANSFERMSG t_msg;
    CONNMSG c_msg;
    t_msg._action = TRANSFERACTION::req;
    t_msg._fmodule = TRANSFERMODULE::mmeeting;
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

void RTTransferSession::SendTransferData(const char* pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

// from RTTcp
void RTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    {
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
    
    {
        int parsed = 0;
        parsed = RTTransfer::ProcessData(m_pBuffer, m_nBufOffset);
        
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

void RTTransferSession::OnLcsEvent()
{

}

void RTTransferSession::OnPeerEvent()
{

}

void RTTransferSession::OnTickEvent()
{

}

// from RTTransfer

void RTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
}

void RTTransferSession::OnTypeConn(TRANSFERMODULE fmodule, const std::string& str)
{
    CONNMSG c_msg;
    std::string err;
    c_msg.GetMsg(str, err);
    if (err.length() > 0) {
        //connid error
        LE("%s invalid params error\n");
        return;
    }
    if ((c_msg._tag == CONNTAG::co_id) && c_msg._msg.compare("hello") == 0) {
        // when ME connector to other:
        if (c_msg._id.length()>0) {
            m_transferSessId = c_msg._id;
            {
                RTConnectionManager::ModuleInfo* pmi = new RTConnectionManager::ModuleInfo();
                if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = fmodule;
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                RTConnectionManager::Instance()->AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                RTConnectionManager::Instance()->AddTypeModuleSession(fmodule, c_msg._moduleid, m_transferSessId);
                LI("store other moduleid:%s, transfersessionid:%s\n", c_msg._moduleid.c_str(), m_transferSessId.c_str());
                } else {
                    LE("new ModuleInfo error\n");
                }
            }
            TRANSFERMSG t_msg;
            
            t_msg._action = TRANSFERACTION::req;
            t_msg._fmodule = TRANSFERMODULE::mmeeting;
            t_msg._type = TRANSFERTYPE::conn;
            t_msg._trans_seq = GenericTransSeq();
            t_msg._trans_seq_ack = 0;
            t_msg._valid = 1;
            
            c_msg._tag = CONNTAG::co_msgid;
            c_msg._id = m_transferSessId;
            c_msg._msgid = "ok";
            //send self module id to other
            c_msg._moduleid = RTConnectionManager::Instance()->MeetingId();
            
            t_msg._content = c_msg.ToJson();
            
            std::string s = t_msg.ToJson();
            SendTransferData(s.c_str(), (int)s.length());
            
        } else {
            LE("Connection id:%s error!!!\n", c_msg._id.c_str());
        }
    }
}

void RTTransferSession::OnTypeTrans(TRANSFERMODULE fmodule, const std::string& str)
{
    TRANSMSG t_msg;
    std::string err;
    t_msg.GetMsg(str, err);
    if (err.length() > 0) {
        LE("%s TRANSMSG error:%s\n", __FUNCTION__, err.c_str());
        return;
    }
    //the users connection lost
    if (t_msg._flag == 1) {
        OnConnectionLostNotify(t_msg._touser);
        LE("user %s lost connection\n", t_msg._touser.c_str());
        return;
    }
    MEETMSG m_mmmsg;
    err = "";
    m_mmmsg.GetMsg(t_msg._content.c_str(), err);
    if (err.length() > 0) {
        LE("%s MEETMSG error:%s\n", __FUNCTION__, err.c_str());
        return;
    }
    switch (m_mmmsg._cmd) {
        case MEETCMD::enter:
        case MEETCMD::leave:
        {
            RTRoomManager::Instance()->HandleOptRoom(t_msg, m_mmmsg);
        }
            break;
        case MEETCMD::dcomm:
        {
            //handle msgs
            std::string tos, res;
            RTRoomManager::Instance()->HandleDcommRoom(t_msg, m_mmmsg);
        }
            break;
        default:
        {
            //invalid params
            LE("OnTypeTrans invalid params\n");
        }
            break;
    }
}

void RTTransferSession::OnTypeQueue(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void RTTransferSession::OnTypeDispatch(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void RTTransferSession::OnTypePush(TRANSFERMODULE fmodule, const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

/**
 *  User connection lost, clear user in the meeting
 */
void RTTransferSession::OnConnectionLostNotify(const std::string& uid)
{
    RTRoomManager::Instance()->ClearSessionLost(uid);
}

/**
 * TransferSession connection lost, clear Transfer Session
 */
void RTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        RTConnectionManager::Instance()->TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

void RTTransferSession::GenericMsgId(std::string& strId)
{
    char buf[32] = {0};
    int id_ = (UInt32)atomic_add(&g_msg_id, 1);
    sprintf(buf, "meet_%06d", id_);
    strId = buf;
}

int RTTransferSession::GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
}

void RTTransferSession::EstablishAck()
{

}

void RTTransferSession::OnEstablishConn()
{

}

void RTTransferSession::OnEstablishAck()
{

}
