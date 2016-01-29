//
//  CRTDispatchConnection.cpp
//  dyncRTConnector
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTDispatchConnection.h"
#include "CRTConnectionManager.h"
#include "CRTConnection.h"
#include "CRTConnectionTcp.h"
#include "rtklog.h"
#include "RTSignalMsg.h"

std::string CRTDispatchConnection::m_connIp;
std::string CRTDispatchConnection::m_connPort;

bool CRTDispatchConnection::IsUserLive(const std::string& uid)
{
    return true;
}

void CRTDispatchConnection::DispatchMsg(const std::string& uid, const std::string& msg)
{
    //find connector
    CRTConnectionManager::ConnectionInfo* pci = CRTConnectionManager::Instance()->findConnectionInfoById(uid);
    if (!pci) {
        LI("Dispatch user:%s not login, need push msg\n", uid.c_str());
        //PushMsg(uid, msg);
        return;
    } else { //!pci
        if (pci->_pConn && pci->_pConn->IsLiveSession()) {
            if (pci->_connType == CONNECTIONTYPE::_chttp) {
                CRTConnection *c = dynamic_cast<CRTConnection*>(pci->_pConn);
                if (c) {
                    c->SendDispatch(uid, msg);
                }
            } else if (pci->_connType == CONNECTIONTYPE::_ctcp) {
                CRTConnectionTcp *ct = dynamic_cast<CRTConnectionTcp*>(pci->_pConn);
                if (ct) {
                    ct->SendDispatch(uid, msg);
                }
            }
        } else { //pci->pConn
            LE("DispatchMsg user:%s session not live, need push msg\n", uid.c_str());
            //PushMsg(uid, msg);
        }
    }
    
}


void CRTDispatchConnection::PushMsg(const std::string& uid, const std::string& msg)
{
    MEETMSG m_msg;
    std::string err("");
    m_msg.GetMsg(msg, err);
    if (err.length()>0) {
        LE("CRTDispatchConnection::PushMsg get MeetMsg err:%s\n", err.c_str());
        return;
    }
    if (m_msg._tags == SENDTAGS::sendtags_talk) {
        std::string no = m_msg._rname + ": " + m_msg._nname + " send a msg!";
        //LI("CRTDispatchConnection::PushMsg talk uid:%s, cont:%s\n", uid.c_str(), m_msg._cont.c_str());
        CRTConnectionManager::Instance()->PushMeetingMsg(m_msg._pass, m_msg._room, m_msg._cont, no);
    } else if (m_msg._tags == SENDTAGS::sendtags_enter) {
        std::string no = m_msg._nname + " enter room " + m_msg._rname + " and is waiting for you!";
        //LI("CRTDispatchConnection::PushMsg enter uid:%s, cont:%s\n", uid.c_str(), m_msg._cont.c_str());
        CRTConnectionManager::Instance()->PushMeetingMsg(m_msg._pass, m_msg._room, m_msg._cont, no);
    }
    
}
