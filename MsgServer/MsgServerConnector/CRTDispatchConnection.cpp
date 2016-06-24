//
//  CRTDispatchConnection.cpp
//  dyncRTConnector
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTDispatchConnection.h"
#include "CRTConnManager.h"
#include "CRTConnection.h"
#include "CRTConnectionTcp.h"
#include "rtklog.h"

std::string CRTDispatchConnection::m_connIp;
std::string CRTDispatchConnection::m_connPort;

void CRTDispatchConnection::DispatchMsg(const std::string& uid, const std::string& msg)
{
    //find connector
    CRTConnManager::ConnectionInfo* pci = CRTConnManager::Instance().findConnectionInfoById(uid);
    if (!pci) {
        LE("DispatchMsg not find user:%s connection\n", uid.c_str());
        return;
    } else { //!pci
        if (pci->_pConn && pci->_pConn->IsLiveSession()) {
            if (pci->_connType == pms::EConnType::THTTP) {
                CRTConnection *c = dynamic_cast<CRTConnection*>(pci->_pConn);
                if (c) {
                    c->SendDispatch(uid, msg);
                }
            } else if (pci->_connType == pms::EConnType::TTCP) {
                CRTConnectionTcp *ct = dynamic_cast<CRTConnectionTcp*>(pci->_pConn);
                if (ct) {
                    ct->SendDispatch(uid, msg);
                }
            }
        }
    }

}
