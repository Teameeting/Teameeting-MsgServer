//
//  RTConnectionTcp.cpp
//  dyncRTConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTConnectionTcp.h"
#include "RTConnectionManager.h"
#include "RTHiredis.h"
#include "atomic.h"
#include "StatusCode.h"

static unsigned int	g_trans_id = 0;

RTConnectionTcp::RTConnectionTcp()
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
{
    m_nBufLen = kRequestBufferSizeInBytes;
    m_pBuffer = new char[m_nBufLen];
    AddObserver(this);
}

RTConnectionTcp::~RTConnectionTcp()
{
    DelObserver(this);
    delete[] m_pBuffer;
}

int RTConnectionTcp::SendDispatch(const std::string &id, const std::string &msg)
{
    RTConnTcp::SendResponse(200, msg);
    return 0;
}

void RTConnectionTcp::GenericResponse(SIGNALTYPE stype, MSGTYPE mtype, long long mseq, int code, const std::string& status, std::string& resp)
{
    MEETMSG m_msg;
    
    m_msg._mtype = mtype;
    m_msg._messagetype = MESSAGETYPE::response;
    m_msg._signaltype = stype;
    m_msg._cmd = 0;
    m_msg._from = "";
    m_msg._room = "";
    m_msg._to = "";
    m_msg._action = 0;
    m_msg._tags = 0;
    m_msg._type = 0;
    m_msg._cont = "";
    m_msg._pass = "";
    m_msg._mseq = mseq;
    m_msg._code = code;
    m_msg._status = status;
    m_msg._nmem = 0;
    m_msg._ntime = OS::Milliseconds();
    
    resp = m_msg.ToJson();
}

//* For RCTcp
void RTConnectionTcp::OnRecvData(const char*pData, int nLen)
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
        parsed = RTConnTcp::ProcessData(m_pBuffer, m_nBufOffset);
        
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
        } else {
            LE("OnRecvDaa parsed <=0 error!!!\n");
        }
    }
}

void RTConnectionTcp::OnLcsEvent()
{

}


//* For RTConnTcp
void RTConnectionTcp::OnLogin(const char* pUserid, const char* pPass)
{
    {
        //check & auth
        
        //std::string uid;
        //m_rtHiredis.CmdGet(pPass, uid);
        //if (uid.length()>0 && uid.compare(pUserid) == 0) {
        //login ok send response
        //} else {
        //login failed
        //}
    }
    std::string sid;
    {
        //store userid & pass
        RTConnectionManager::ConnectionInfo* pci = new RTConnectionManager::ConnectionInfo();
        if (pci) {
            RTConnectionManager::Instance()->GenericSessionId(sid);
            m_connectorId = RTConnectionManager::Instance()->ConnectorId();
            pci->connId = sid;
            pci->connAddr = RTConnectionManager::Instance()->ConnectorIp();
            pci->connPort = RTConnectionManager::Instance()->ConnectorPort();
            pci->userId = pUserid;
            pci->pConn = this;
            pci->connType = CONNECTIONTYPE::_ctcp;
            pci->flag = 1;
            std::string uid(pUserid);
            LI("OnLogin Uid:%s\n", uid.c_str());
            RTConnectionManager::Instance()->AddUser(CONNECTIONTYPE::_ctcp, uid, pci);
            m_userId = pUserid;
        } else {
            LE("new ConnectionInfo error!!!\n");
            std::string resp;
            GenericResponse(SIGNALTYPE::login, MSGTYPE::meeting, 0, RTCommCode::_errconninfo, GetRTCommStatus(RTCommCode::_errconninfo), resp);
            SendResponse(0, resp.c_str());
            return;

        }
    }
    
    {
        // send response
        std::string resp;
        GenericResponse(SIGNALTYPE::login, MSGTYPE::meeting, 0, RTCommCode::_ok, GetRTCommStatus(RTCommCode::_ok), resp);
        SendResponse(0, resp.c_str());
        return;
    }
}

void RTConnectionTcp::OnSndMsg(MSGTYPE mType, long long mseq, const char* pUserid, const char* pData, int dLen)
{
    if (!pData) {
        LE("%s invalid params\n", __FUNCTION__);
        std::string resp;
        GenericResponse(SIGNALTYPE::sndmsg, mType, mseq, RTCommCode::_invparams, GetRTCommStatus(RTCommCode::_invparams), resp);
        SendResponse(0, resp.c_str());
        return;
    }
    
    //no matter mType is meet or callcenter or p2p or others
    //the following code should be same
    //find an TrasnferSession By mtype
    //transfer msg by TransferSession
    
    TRANSFERMSG t_trmsg;
    TRANSMSG t_msg;
    t_msg._flag = 0;
    t_msg._touser = "";
    t_msg._connector = RTConnectionManager::Instance()->ConnectorId();
    t_msg._content = pData;
    
    t_trmsg._action = TRANSFERACTION::req;
    t_trmsg._fmodule = TRANSFERMODULE::mconnector;
    t_trmsg._type = TRANSFERTYPE::trans;
    t_trmsg._trans_seq = GenericTransSeq();
    t_trmsg._trans_seq_ack = 0;
    t_trmsg._valid = 1;
    t_trmsg._content = t_msg.ToJson();
    
    const std::string s = t_trmsg.ToJson();
    RTConnectionManager::ModuleInfo* pmi = RTConnectionManager::Instance()->findModuleInfo(pUserid, (TRANSFERMODULE)mType);
    if (pmi && pmi->pModule) {
        pmi->pModule->SendTransferData(s.c_str(), (int)s.length());
    } else {
        LE("pmi->pModule is NULL\n");
        std::string resp;
        GenericResponse(SIGNALTYPE::sndmsg, mType, mseq, RTCommCode::_errmoduinfo, GetRTCommStatus(RTCommCode::_errmoduinfo), resp);
        SendResponse(0, resp.c_str());
        return;
    }
    {
        std::string resp;
        GenericResponse(SIGNALTYPE::sndmsg, mType, mseq, RTCommCode::_ok, GetRTCommStatus(RTCommCode::_ok), resp);
        SendResponse(0, resp.c_str());
        return;
    }
}

void RTConnectionTcp::OnGetMsg(MSGTYPE mType, long long mseq, const char* pUserid)
{
    std::string resp;
    GenericResponse(SIGNALTYPE::getmsg, mType, mseq, RTCommCode::_ok, GetRTCommStatus(RTCommCode::_ok), resp);
    SendResponse(0, resp.c_str());
    return;
}

void RTConnectionTcp::OnLogout(const char* pUserid)
{
    std::string uid(pUserid);
    RTConnectionManager::Instance()->DelUser(CONNECTIONTYPE::_ctcp, uid);
    m_userId = "";
    std::string resp;
    GenericResponse(SIGNALTYPE::logout, MSGTYPE::meeting, 1, RTCommCode::_ok, GetRTCommStatus(RTCommCode::_ok), resp);
    SendResponse(0, resp.c_str());
    return;
}

void RTConnectionTcp::OnKeepAlive(const char *pUserid)
{
    LI("RTConnectionTcp::OnKeepAlive pUserid:%s\n", pUserid);
    RTTcp::UpdateTimer();
    {
        std::string resp;
        GenericResponse(SIGNALTYPE::keepalive, MSGTYPE::meeting, 2, RTCommCode::_ok, GetRTCommStatus(RTCommCode::_ok), resp);
        SendResponse(0, resp.c_str());
        return;
    }
}

void RTConnectionTcp::OnResponse(const char*pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
}

void RTConnectionTcp::ConnectionDisconnected()
{
    if (m_userId.length()) {
        LI("RTConnectionTcp::ConnectionDisconnected DelUser m_userId:%s\n", m_userId.c_str());
        RTConnectionManager::Instance()->ConnectionLostNotify(m_userId);
    } else {
        LE("RTConnectionTcp::ConnectionDisconnected m_userId.length is 0\n");
    }
}


///////////////private///////////////////

int RTConnectionTcp::GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
}
