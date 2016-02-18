//
//  DRTTransferSession.h
//  MsgServerDispatcher
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerDispatcher__DRTTransferSession__
#define __MsgServerDispatcher__DRTTransferSession__

#include <stdio.h>
#include <iostream>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcp.h"
#include "RTJSBuffer.h"
#include "RTTransfer.h"
#include "RTObserverConnection.h"

class DRTTransferSession
    : public RTTcp
    , public RTJSBuffer
    , public RTTransfer
    , public RTObserverConnection{
public:
    DRTTransferSession();
    virtual ~DRTTransferSession();
    void Init();
    void Unit();
    bool Connect(const std::string addr, int port);
    void Disconn();
    bool RefreshTime();
    void KeepAlive();
    
    void SendTransferData(const char* pData, int nLen);
    void SetModuleId(std::string& moduleId) { m_moduleId = moduleId; }
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

protected:
   virtual void OnRecvMessage(const char*message, int nLen);
// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();
        
private:
    void GenericMsgId(std::string& strId);
    int  GenericTransSeq();
    void EstablishAck();
    void OnEstablishConn();
    void OnEstablishAck();
private:
    std::string     m_transferSessId;
    UInt64          m_lastUpdateTime;
    std::string     m_moduleId;
};

#endif /* defined(__MsgServerDispatcher__DRTTransferSession__) */
