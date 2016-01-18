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

void CRTDispatchConnection::DispatchMsg(const std::string& uid, const std::string& msg)
{
    //find connector
    OSMutexLocker locker(&m_mutex);
    CRTConnectionManager::ConnectionInfo* pci = CRTConnectionManager::Instance()->findConnectionInfoById(uid);
    if (!pci) {
        LI("Dispatch user:%s not login, need push msg\n", uid.c_str());
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
        }
    }
    
}
