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

#include "MsgServer/proto/entity_msg.pb.h"
#include "MsgServer/proto/entity_msg_type.pb.h"


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

void CRTConnectionTcp::GenericResponse(pms::EServerCmd cmd, pms::EModuleType module, int code, std::string& result, std::string& resp)
{
#if DEF_PROTO
    pms::MsgRep response;

    response.set_svr_cmds(cmd);
    response.set_mod_type(module);
    response.set_rsp_code(code);
    response.set_rsp_cont(result);
    resp = response.SerializeAsString();

#else
    LE("not define DEF_PROTO\n");
#endif
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
void CRTConnectionTcp::OnLogin(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    pms::MsgReq request;
    if (!request.ParseFromString(msg)) {
        LE("request.ParseFromString MsgReq error\n");
        return;
    }

    pms::Login login;
    if (!login.ParseFromString(request.content())) {
        LE("login.ParseFromString Login error\n");
        return;
    }

    m_userId = login.usr_from();
    m_token = login.usr_token();
    m_nname = login.usr_nname();

    CRTConnManager::Instance().TransferToPusher(cmd, module, m_userId, msg);
    LI("Onlogin user:%s login\n", m_userId.c_str());
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
            pci->_userId = m_userId;
            pci->_token = m_token;
            pci->_pConn = this;
            pci->_connType = pms::EConnType::TTCP;
            pci->_flag = 1;
            CRTConnManager::Instance().AddUser(pms::EConnType::TTCP, m_userId, pci);
            CRTConnManager::Instance().ConnectionConnNotify(m_userId, m_token);

            // send response
            std::string resp;
            std::string result("login ok");
            GenericResponse(pms::EServerCmd::CLOGIN, module, 0, result, resp);
            SendResponse(0, resp.c_str());
            m_login = true;
            return;
        } else {
            LE("new ConnectionInfo error userid:%s\n", m_userId.c_str());
            std::string resp;
            std::string result("login failed");
            GenericResponse(pms::EServerCmd::CLOGIN, module, 101, result, resp);
            SendResponse(0, resp.c_str());
            m_login = false;
            return;
        }
    }
#else
    LE("not define DEF_PROTO\n");
#endif

}

void CRTConnectionTcp::OnSndMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    if (!m_login) {
        LE("m_login false, can not transfer msg\n");
        return;
    }

    //no matter mType is meet or callcenter or p2p or others
    //the following code should be same
    //find an TrasnferSession By module
    //transfer msg by TransferSession

    // enclosing msg in CRTTransferSession::TransferMsg
    pms::Entity request;
    request.ParseFromString(msg);
    CRTConnManager::Instance().TransferMsg(cmd, module, m_userId, msg);
    std::string resp;
    GenericResponse(pms::EServerCmd::CSNDMSG, module, 0, *request.mutable_cmsg_id(), resp);
    SendResponse(0, resp.c_str());
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTConnectionTcp::OnGetMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{

}

void CRTConnectionTcp::OnLogout(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    pms::Logout logout;
    if (!logout.ParseFromString(msg)) {
        LE("login.ParseFromString error\n");
    }
    assert(logout.usr_from().compare(m_userId)==0);
    LI("OnLogout user:%s logout\n", m_userId.c_str());
    assert(logout.usr_from().compare(m_userId)==0);
    std::string token;
    CRTConnManager::Instance().DelUser(pms::EConnType::TTCP, m_userId, token);
    CRTConnManager::Instance().ConnectionLostNotify(m_userId, m_token);

    m_userId = "";
    m_token = "";
    m_nname = "";
    std::string resp;
    std::string result("logout ok");
    GenericResponse(pms::EServerCmd::CLOGOUT, module, 0, result, resp);
    SendResponse(0, resp.c_str());
    m_login = false;
    return;
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTConnectionTcp::OnKeepAlive(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    if (m_login) {
        pms::Keep keep;
        if (!keep.ParseFromString(msg)) {
            LE("login.ParseFromString error\n");
        }
        LI("Userid:%s OnKeepAlive\n", keep.usr_from().c_str());
        RTTcp::UpdateTimer();
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTConnectionTcp::OnSyncSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    if (m_login) {
        CRTConnManager::Instance().TransferMsg(cmd, module, m_userId, msg);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTConnectionTcp::OnSyncData(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    if (m_login) {
        CRTConnManager::Instance().TransferMsg(cmd, module, m_userId, msg);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTConnectionTcp::OnCreateSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    if (m_login) {
        CRTConnManager::Instance().TransferMsg(cmd, module, m_userId, msg);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTConnectionTcp::OnDeleteSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    if (m_login) {
        CRTConnManager::Instance().TransferMsg(cmd, module, m_userId, msg);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTConnectionTcp::OnUpdateSetting(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg)
{
#if DEF_PROTO
    if (m_login) {
        CRTConnManager::Instance().TransferToPusher(cmd, module, m_userId, msg);
    }
#else
    LE("not define DEF_PROTO\n");
#endif
}

void CRTConnectionTcp::OnResponse(const char*pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
}

void CRTConnectionTcp::ConnectionDisconnected()
{
    if (m_userId.length()>0) {
        LI("ConnectionDisconnected user m_userId:%s, m_token:%s disconnected!\n", m_userId.c_str(), m_token.c_str());
        std::string token;
        CRTConnManager::Instance().DelUser(pms::EConnType::THTTP, m_userId, token);
        CRTConnManager::Instance().ConnectionLostNotify(m_userId, m_token);
    }
}


///////////////private///////////////////
