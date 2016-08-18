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

//void CRTDispatchConnection::DispatchMsg(const std::string& uid, const std::string& msg, const std::string& handle_cmd, const std::string& handle_mtype, const std::string& handle_data)
void CRTDispatchConnection::DispatchMsg(const std::string& uid, pms::RelayMsg& r_msg)
{
    //find connector
    CRTConnManager::ConnectionInfo* pci = CRTConnManager::Instance().findConnectionInfoById(uid);
    if (!pci) {
        LE("DispatchMsg not find user:%s connection\n", uid.c_str());
        LI("CRTTransferSession::DispatchMsg handle_cmd:%s, handle_mtype:%s, handle_data:%s\n", r_msg.handle_cmd().c_str(), r_msg.handle_mtype().c_str(), r_msg.handle_data().c_str());

        return;
    } else { //!pci
        if (pci->_pConn && pci->_pConn->IsLiveSession()) {
            if (pci->_connType == pms::EConnType::THTTP) {
                CRTConnection *c = dynamic_cast<CRTConnection*>(pci->_pConn);
                if (c) {
                    c->SendDispatch(uid, r_msg.content());
                }
            } else if (pci->_connType == pms::EConnType::TTCP) {
                CRTConnectionTcp *ct = dynamic_cast<CRTConnectionTcp*>(pci->_pConn);
                if (ct) {
                    ct->SendDispatch(uid, r_msg.content());
                }
            }
        }
    }

}
