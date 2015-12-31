//
//  RTConnectorConnection.cpp
//  dyncRTMsgQueue
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTConnectorConnection.h"
#include "RTConnectionManager.h"
#include "rtklog.h"

std::string RTConnectorConnection::m_msgQueueIp;
std::string RTConnectorConnection::m_msgQueuePort;

void RTConnectorConnection::SendToConnector(const std::string& sid, const std::string& uid, const std::string& msg)
{
    RTConnectionManager::ModuleInfo mi = RTConnectionManager::Instance()->findModuleInfoByType(TRANSFERMODULE::mconnector);
    mi.p_module->SendTransferData(msg.c_str(), (int)msg.length());
}
