//
//  RTDispatchConnection.cpp
//  dyncRTConnector
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTDispatchConnection.h"
#include "RTConnectionManager.h"
#include "RTConnection.h"
#include "RTConnectionTcp.h"
#include "rtklog.h"
#include "RTSignalMsg.h"

std::string RTDispatchConnection::m_connIp;
std::string RTDispatchConnection::m_connPort;

void RTDispatchConnection::DispatchMsg(const std::string& uid, const std::string& msg)
{
    std::string cid;
    //find connector
    RTConnectionManager::ConnectionInfo* pci = RTConnectionManager::Instance()->findConnectionInfoById(uid);
    if (pci && pci->pConn && pci->pConn->IsLiveSession() && pci->connType == CONNECTIONTYPE::_chttp) {
        RTConnection *c = dynamic_cast<RTConnection*>(pci->pConn);
        if (c) {
            c->SendDispatch(uid, msg);
        }
    } else if (pci && pci->pConn && pci->pConn->IsLiveSession() && pci->connType == CONNECTIONTYPE::_ctcp) {
        RTConnectionTcp *ct = dynamic_cast<RTConnectionTcp*>(pci->pConn);
        if (ct) {
            ct->SendDispatch(uid, msg);
        }
    } else {
        LE("DispatchMsg error\n");
        RTConnectionManager::Instance()->DelUser(CONNECTIONTYPE::_chttp, uid);
    }
}
