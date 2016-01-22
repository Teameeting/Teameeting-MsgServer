//
//  CRTConnectionTcp.cpp
//  dyncRTConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTConnectionTcp.h"
#include "CRTConnectionManager.h"
#include "atomic.h"
#include "StatusCode.h"

static unsigned int	g_trans_id = 0;

CRTConnectionTcp::CRTConnectionTcp()
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
{
    m_nBufLen = kRequestBufferSizeInBytes;
    m_pBuffer = new char[m_nBufLen];
    AddObserver(this);
}

CRTConnectionTcp::~CRTConnectionTcp()
{
    DelObserver(this);
    delete[] m_pBuffer;
}

int CRTConnectionTcp::SendDispatch(const std::string &id, const std::string &msg)
{
    CRTConnTcp::SendResponse(200, msg);
    return 0;
}

void CRTConnectionTcp::GenericResponse(SIGNALTYPE stype, MSGTYPE mtype, long long mseq, int code, const std::string& status, std::string& resp)
{
    MEETMSG m_msg;

    m_msg._mtype = mtype;
    m_msg._messagetype = MESSAGETYPE::response;
    m_msg._signaltype = stype;
    m_msg._cmd = MEETCMD::meetcmd_invalid;
    m_msg._from = "";
    m_msg._room = "";
    m_msg._to = "";
    m_msg._action = DCOMMACTION::dcommaction_invalid;
    m_msg._tags = SENDTAGS::sendtags_invalid;
    m_msg._type = SENDTYPE::sendtype_invalid;
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
void CRTConnectionTcp::OnRecvData(const char*pData, int nLen)
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
        parsed = CRTConnTcp::ProcessData(m_pBuffer, m_nBufOffset);

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

void CRTConnectionTcp::OnLcsEvent()
{

}


//* For RTConnTcp
void CRTConnectionTcp::OnLogin(const char* pUserid, const char* pPass)
{
    {
        //check & auth
        /*
        std::string pass;
        RTHiredisRemote::Instance()->CmdGet(pUserid, pass);
        if (pass.compare(pPass)!=0) {
            LE("OnLogin user pass has error, pUserid:%s, pPass:%s, pass:%s\n", pUserid, pPass, pass.c_str());
            // send response
            std::string resp;
            GenericResponse(SIGNALTYPE::login, MSGTYPE::meeting, 0, RTCommCode::_invparams, GetRTCommStatus(RTCommCode::_invparams), resp);
            SendResponse(0, resp.c_str());
            return;
        }*/
    }
    m_userId = pUserid;
    m_token = pPass;
    std::string sid;
    {
        //store userid & pass
        CRTConnectionManager::ConnectionInfo* pci = new CRTConnectionManager::ConnectionInfo();
        if (pci) {
            CRTConnectionManager::Instance()->GenericSessionId(sid);
            LI("==============GenericSessioNid sid:%s\n", sid.c_str());
            m_connectorId = CRTConnectionManager::Instance()->ConnectorId();
            pci->_connId = sid;
            pci->_connAddr = CRTConnectionManager::Instance()->ConnectorIp();
            pci->_connPort = CRTConnectionManager::Instance()->ConnectorPort();
            pci->_userId = pUserid;
            pci->_token = pPass;
            pci->_pConn = this;
            pci->_connType = CONNECTIONTYPE::_ctcp;
            pci->_flag = 1;
            std::string uid(pUserid);
            LI("OnLogin Uid:%s\n", uid.c_str());
            CRTConnectionManager::Instance()->AddUser(CONNECTIONTYPE::_ctcp, uid, pci);
            // send response
            std::string resp;
            GenericResponse(SIGNALTYPE::login, MSGTYPE::meeting, 0, RTCommCode::_ok, GetRTCommStatus(RTCommCode::_ok), resp);
            SendResponse(0, resp.c_str());
            return;
        } else {
            LE("new ConnectionInfo error!!!\n");
            std::string resp;
            GenericResponse(SIGNALTYPE::login, MSGTYPE::meeting, 0, RTCommCode::_errconninfo, GetRTCommStatus(RTCommCode::_errconninfo), resp);
            SendResponse(0, resp.c_str());
            return;

        }
    }

}

void CRTConnectionTcp::OnSndMsg(MSGTYPE mType, long long mseq, const char* pUserid, const char* pData, int dLen)
{
    if (!pData) {
        LE("%s invalid params\n", __FUNCTION__);
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
    t_msg._connector = CRTConnectionManager::Instance()->ConnectorId();
    t_msg._content = pData;

    t_trmsg._action = TRANSFERACTION::req;
    t_trmsg._fmodule = TRANSFERMODULE::mconnector;
    t_trmsg._type = TRANSFERTYPE::trans;
    t_trmsg._trans_seq = GenericTransSeq();
    t_trmsg._trans_seq_ack = 0;
    t_trmsg._valid = 1;
    t_trmsg._content = t_msg.ToJson();

    const std::string s = t_trmsg.ToJson();
    CRTConnectionManager::ModuleInfo* pmi = CRTConnectionManager::Instance()->findModuleInfo(pUserid, (TRANSFERMODULE)mType);
    if (pmi && pmi->pModule) {
        pmi->pModule->SendTransferData(s.c_str(), (int)s.length());
    } else {
        LE("pmi->pModule is NULL\n");
        Assert(false);
        return;
    }
}

void CRTConnectionTcp::OnGetMsg(MSGTYPE mType, long long mseq, const char* pUserid)
{

}

void CRTConnectionTcp::OnLogout(const char* pUserid)
{
    if (!pUserid) {
        return;
    }
    std::string token;
    CRTConnectionManager::Instance()->DelUser(CONNECTIONTYPE::_ctcp, pUserid, token);
    m_userId.assign("");
    m_token.assign("");
    std::string resp;
    GenericResponse(SIGNALTYPE::logout, MSGTYPE::meeting, 1, RTCommCode::_ok, GetRTCommStatus(RTCommCode::_ok), resp);
    SendResponse(0, resp.c_str());
    return;
}

void CRTConnectionTcp::OnKeepAlive(const char *pUserid)
{
    LI("RTConnectionTcp::OnKeepAlive pUserid:%s\n", pUserid);
    RTTcp::UpdateTimer();
}

void CRTConnectionTcp::OnResponse(const char*pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
}

void CRTConnectionTcp::ConnectionDisconnected()
{
    if (m_userId.length()>0) {
        LI("RTConnectionTcp::ConnectionDisconnected DelUser m_userId:%s, m_token:%s\n", m_userId.c_str(), m_token.c_str());
        std::string token;
        CRTConnectionManager::Instance()->DelUser(CONNECTIONTYPE::_ctcp, m_userId, token);
        CRTConnectionManager::Instance()->ConnectionLostNotify(m_userId, m_token);
    } else {
        LE("RTConnectionTcp::ConnectionDisconnected m_userId.length is 0\n");
        Assert(false);
    }
}


///////////////private///////////////////

int CRTConnectionTcp::GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
}
