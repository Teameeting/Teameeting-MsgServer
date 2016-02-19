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
    
    void TestConnection();
        
public:
    void EstablishConnection();
    
// from RTTcp
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnWakeupEvent();
    virtual void OnPushEvent();
    virtual void OnTickEvent();
    
// from RTTransfer
public:
    virtual void OnTransfer(const std::string& str);
    virtual void OnMsgAck(TRANSFERMSG& tmsg);
    virtual void OnTypeConn(TRANSFERMODULE fmodule, const std::string& str);
    virtual void OnTypeTrans(TRANSFERMODULE fmodule, const std::string& str);
    virtual void OnTypeQueue(TRANSFERMODULE fmodule, const std::string& str);
    virtual void OnTypeDispatch(TRANSFERMODULE fmodule, const std::string& str);
    virtual void OnTypePush(TRANSFERMODULE fmodule, const std::string& str);
        
// from RTObserverConnection
    virtual void ConnectionDisconnected();
protected:
    virtual void OnRecvMessage(const char*message, int nLen);
private:
    std::string     m_transferSessId;
    CRTDispatchConnection  m_dispatchConnection;
    
};

#endif /* defined(__MsgServerConnector__CRTTransferSession__) */
