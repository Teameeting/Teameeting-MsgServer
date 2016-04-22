//
//  MRTTransferSession.h
//  MsgServerMeeting
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerMeeting__MRTTransferSession__
#define __MsgServerMeeting__MRTTransferSession__

#include <stdio.h>
#include <iostream>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcp.h"
#include "RTJSBuffer.h"
#include "RTTransfer.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1
#include "MsgServer/MSCommon/MSProtocol/proto/msg_type.pb.h"
#include "MsgServer/MSCommon/MSProtocol/proto/meet_msg.pb.h"
#include "MsgServer/MSCommon/MSProtocol/proto/sys_msg.pb.h"

class MRTTransferSession
    : public RTTcp
    , public RTJSBuffer
    , public RTTransfer
    , public RTObserverConnection{
public:
    MRTTransferSession(pms::ETransferModule module);
    virtual ~MRTTransferSession();
    void Init();
    void Unit();
    bool Connect(const std::string addr, int port);
    bool Connect();
    void Disconn();
    bool RefreshTime();
    void KeepAlive();

    void SendTransferData(const char* pData, int nLen);
    void SetModuleId(std::string& moduleId) { m_moduleId = moduleId; }
    void TestConnection();

    std::string& GetTransferAddr() { return m_addr; }
    int GetTransferPort() { return m_port; }
    int GetConnectingStatus() { return m_connectingStatus; };

public:
    void EstablishConnection();
    void OnConnectionLostNotify(const std::string& uid, const std::string& token, const std::string& connector);

// from RTTcp
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen) {}

// from RTTransfer
public:
    virtual void OnTransfer(const std::string& str);
    virtual void OnMsgAck(pms::TransferMsg& tmsg);
    virtual void OnTypeConn(const std::string& str);
    virtual void OnTypeTrans(const std::string& str);
    virtual void OnTypeQueue(const std::string& str);
    virtual void OnTypeDispatch(const std::string& str);
    virtual void OnTypePush(const std::string& str);
    virtual void OnTypeTLogin(const std::string& str);
    virtual void OnTypeTLogout(const std::string& str);

protected:
    virtual void OnRecvMessage(const char*message, int nLen);
// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();

private:
    std::string     m_transferSessId;
    UInt64          m_lastUpdateTime;
    std::string     m_moduleId;
    pms::ETransferModule  m_module;
    std::string     m_addr;
    int             m_port;
    int             m_connectingStatus;
};

#endif /* defined(__MsgServerMeeting__MRTTransferSession__) */
