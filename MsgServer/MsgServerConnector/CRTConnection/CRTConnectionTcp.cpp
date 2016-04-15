//
//  CRTConnectionTcp.cpp
//  dyncRTConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTConnectionTcp.h"
#include "CRTConnManager.h"
#include "StatusCode.h"
#include "RTUtils.hpp"

CRTConnectionTcp::CRTConnectionTcp()
: RTJSBuffer()
, m_connectorId("")
, m_userId("")
, m_token("")
, m_nname("")
, m_login(false)
{
    AddObserver(this);
}

CRTConnectionTcp::~CRTConnectionTcp()
{
    DelObserver(this);
}

int CRTConnectionTcp::SendDispatch(const std::string &id, const std::string &msg)
{
    CRTConnTcp::SendResponse(200, msg);
    return 0;
}

void CRTConnectionTcp::GenericResponse(SIGNALTYPE stype, MSGTYPE mtype, long long mseq, int code, std::string& resp)
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
    m_msg._nname = m_nname;
    m_msg._rname = "";
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
    RTJSBuffer::RecvData(pData, nLen);
}

void CRTConnectionTcp::OnRecvMessage(const char*message, int nLen)
{
    CRTConnTcp::DoProcessData(message, nLen);
}

//* For RTConnTcp
void CRTConnectionTcp::OnLogin(const char* pUserid, const char* pPass, const char* pNname)
{
    {
        //check & auth
        /*
        std::string pass;
        RTHiredisRemote::Instance().CmdGet(pUserid, pass);
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
    m_nname = pNname;
    LI("CRTConnectionTcp::Onlogin pUserid:%s\n", pUserid);
    std::string sid;
    {
        //store userid & pass
        CRTConnManager::ConnectionInfo* pci = new CRTConnManager::ConnectionInfo();
        if (pci) {
            GenericSessionId(sid);
            m_connectorId = CRTConnManager::Instance().ConnectorId();
            pci->_connId = sid;
            pci->_connAddr = CRTConnManager::Instance().ConnectorIp();
            pci->_connPort = CRTConnManager::Instance().ConnectorPort();
            pci->_userId = pUserid;
            pci->_token = pPass;
            pci->_pConn = this;
            pci->_connType = CONNECTIONTYPE::_ctcp;
            pci->_flag = 1;
            CRTConnManager::Instance().AddUser(CONNECTIONTYPE::_ctcp, m_userId, pci);
            CRTConnManager::Instance().ConnectionConnNotify(m_userId, m_token);

            // send response
            std::string resp;
            GenericResponse(SIGNALTYPE::login, MSGTYPE::meeting, 0, RTCommCode::_ok, resp);
            SendResponse(0, resp.c_str());
            m_login = true;
            return;
        } else {
            LE("new ConnectionInfo error userid:%s\n", m_userId.c_str());
            std::string resp;
            GenericResponse(SIGNALTYPE::login, MSGTYPE::meeting, 0, RTCommCode::_errconninfo, resp);
            SendResponse(0, resp.c_str());
            m_login = false;
            return;
        }
    }

}

void CRTConnectionTcp::OnSndMsg(MSGTYPE mType, long long mseq, const char* pUserid, const char* pData, int dLen)
{
    if (!m_login) {
        LE("m_login false, can not transfer msg\n");
        return;
    }
    if (!pData) {
        return;
    }

    //no matter mType is meet or callcenter or p2p or others
    //the following code should be same
    //find an TrasnferSession By mtype
    //transfer msg by TransferSession

    std::string msg(pData, dLen);
    CRTConnManager::Instance().TransferMsg(mType, mseq, pUserid, msg);
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
    CRTConnManager::Instance().DelUser(CONNECTIONTYPE::_ctcp, pUserid, token);
    CRTConnManager::Instance().ConnectionLostNotify(pUserid, m_token);

    m_userId = "";
    m_token = "";
    m_nname = "";
    std::string resp;
    GenericResponse(SIGNALTYPE::logout, MSGTYPE::meeting, 1, RTCommCode::_ok, resp);
    SendResponse(0, resp.c_str());
    m_login = false;
    return;
}

void CRTConnectionTcp::OnKeepAlive(const char *pUserid)
{
    if (m_login) {
        printf("Userid:%s OnKeepAlive\n", pUserid);
        RTTcp::UpdateTimer();
    }
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
        CRTConnManager::Instance().DelUser(CONNECTIONTYPE::_ctcp, m_userId, token);
        CRTConnManager::Instance().ConnectionLostNotify(m_userId, m_token);
    }
}


///////////////private///////////////////
