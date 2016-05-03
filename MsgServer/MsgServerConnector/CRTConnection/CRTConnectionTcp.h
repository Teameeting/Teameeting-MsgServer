//
//  CRTConnectionTcp.h
//  MsgServerConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTConnectionTcp__
#define __MsgServerConnector__CRTConnectionTcp__

#include "RTTcp.h"
#include "CRTConnTcp.h"
#include "RTJSBuffer.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1
#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/meet_msg.pb.h"
#include "MsgServer/proto/meet_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"

class CRTConnectionTcp
: public RTTcp
, public CRTConnTcp
, public RTJSBuffer
, public RTObserverConnection{
public:
    CRTConnectionTcp();
    virtual ~CRTConnectionTcp();
public:
    int SendDispatch(const std::string& id, const std::string& msg);
    void GenericResponse(pms::EServerCmd cmd, pms::EModuleType module, int code, std::string& resp);
public:
    //* For RCTcp
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen) {}

public:
    //* For RTConnTcp
    virtual void OnLogin(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnSndMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnGetMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnLogout(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnKeepAlive(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnResponse(const char*pData, int nLen);
public:
    //* For RTObserverConnection
    virtual void ConnectionDisconnected();

protected:
    virtual void OnRecvMessage(const char*message, int nLen);

private:
    std::string     m_connectorId;
    std::string     m_userId;
    std::string     m_token;
    std::string     m_nname;
    bool            m_login;
};

#endif /* defined(__MsgServerConnector__CRTConnectionTcp__) */
