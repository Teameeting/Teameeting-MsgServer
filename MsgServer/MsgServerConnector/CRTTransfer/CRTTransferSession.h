//
//  CRTTransferSession.h
//  MsgServerConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTTransferSession__
#define __MsgServerConnector__CRTTransferSession__

#include <stdio.h>
#include <iostream>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcp.h"
#include "RTJSBuffer.h"
#include "RTTransfer.h"
#include "CRTDispatchConnection.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class CRTTransferSession
    : public RTTcp
    , public RTJSBuffer
    , public RTTransfer
    , public RTObserverConnection{
public:
    CRTTransferSession();
    virtual ~CRTTransferSession();
    void Init();
    void Unit();
    bool Connect(const std::string addr, int port);
    void Disconn();

    void SendTransferData(const char* pData, int nLen);
    void ConnectionLostNotify(const std::string& uid, const std::string& token);
    void ConnectionConnNotify(const std::string& uid, const std::string& token);
    void TransferMsg(pms::EServerCmd cmd, const std::string& msg);

    void TestConnection();

public:
    void EstablishConnection();

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
    CRTDispatchConnection  m_dispatchConnection;

};

#endif /* defined(__MsgServerConnector__CRTTransferSession__) */
