//
//  SRTTransferSession.h
//  MsgServerStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTTransferSession__
#define __MsgServerStorage__SRTTransferSession__

#include <stdio.h>
#include <iostream>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcpNoTimeout.h"
#include "RTJSBuffer.h"
#include "RTLstorage.h"
#include "RTObserverConnection.h"
#include "SRTRedisManager.h"

#define DEF_PROTO 1
#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/meet_msg.pb.h"
#include "MsgServer/proto/meet_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"
#include "MsgServer/proto/storage_msg.pb.h"

class SRTTransferSession
    : public RTTcpNoTimeout
    , public RTJSBuffer
    , public RTLstorage
    , public RTObserverConnection{
public:
    SRTTransferSession();
    virtual ~SRTTransferSession();
    void Init();
    void InitConf();
    void Unit();
    bool Connect(const std::string addr, int port);
    bool Connect();
    void Disconn();
    bool RefreshTime();
    void KeepAlive();

    void SendTransferData(const char* pData, int nLen);
    void SendTransferData(const std::string& data);

    void SetModuleId(std::string& moduleId) { m_moduleId = moduleId; }
    void TestConnection();

    std::string& GetTransferAddr() { return m_addr; }
    int GetTransferPort() { return m_port; }
    int GetConnectingStatus() { return m_connectingStatus; };
public:
    void EstablishConnection();

// from RTTcpNoTimeout
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

    virtual void OnTypeWriteRequest(const std::string& str);
    virtual void OnTypeWriteResponse(const std::string& str);
    virtual void OnTypeReadRequest(const std::string& str);
    virtual void OnTypeReadResponse(const std::string& str);

protected:
   virtual void OnRecvMessage(const char*message, int nLen);
// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();

private:
    std::string     m_transferSessId;
    UInt64          m_lastUpdateTime;
    std::string     m_moduleId;
    std::string     m_addr;
    int             m_port;
    int             m_connectingStatus;

    SRTRedisManager                     m_RedisManager;
};

#endif /* defined(__MsgServerStorage__SRTTransferSession__) */
