//
//  RRTTransferSession.h
//  MsgServerRTRedis
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTRedis__RRTTransferSession__
#define __MsgServerRTRedis__RRTTransferSession__

#include <stdio.h>
#include <iostream>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcp.h"
#include "RTJSBuffer.h"
#include "RTTransfer.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1
#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/meet_msg.pb.h"
#include "MsgServer/proto/meet_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"

class RRTTransferSession
    : public RTTcp
    , public RTJSBuffer
    , public RTTransfer
    , public RTObserverConnection{
public:
    RRTTransferSession();
    virtual ~RRTTransferSession();
    void Init();
    void Unit();
    bool Connect(const std::string addr, int port);
    void Disconn();

    void SendTransferData(const char* pData, int nLen);
    void TransferMsg(const std::string& msg);

    void TestConnection();
    std::string& GetProcessingMsg() { return m_processingMsg; }

public:

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

// from RTObserverConnection
    virtual void ConnectionDisconnected();
protected:
    virtual void OnRecvMessage(const char*message, int nLen);
private:
    std::string     m_transferSessId;
    std::string     m_processingMsg;

};

#endif /* defined(__MsgServerRTRedis__RRTTransferSession__) */
