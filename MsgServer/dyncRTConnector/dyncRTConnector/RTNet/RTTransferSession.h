//
//  RTTransferSession.h
//  dyncRTConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTConnector__RTTransferSession__
#define __dyncRTConnector__RTTransferSession__

#include <stdio.h>
#include <iostream>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcp.h"
#include "RTTransfer.h"
#include "RTDispatchConnection.h"
#include "RTObserverConnection.h"

class RTTransferSession
    : public RTTcp
    , public RTTransfer
    , public RTObserverConnection{
public:
    RTTransferSession();
    virtual ~RTTransferSession();
    void Init();
    void Unit();
    bool Connect(const std::string addr, int port);
    void Disconn();
    
    void SendTransferData(const char* pData, int nLen);
    void ConnectionLostNotify(const std::string& uid);
    
    void TestConnection();
        
public:
    void EstablishConnection();
    
// from RTTcp
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnLcsEvent();
    virtual void OnPeerEvent();
    virtual void OnTickEvent();
    
// from RTTransfer
public:
    virtual void OnTransfer(const std::string& str);
    virtual void OnTypeConn(TRANSFERMODULE fmodule, const std::string& str);
    virtual void OnTypeTrans(TRANSFERMODULE fmodule, const std::string& str);
    virtual void OnTypeQueue(TRANSFERMODULE fmodule, const std::string& str);
    virtual void OnTypeDispatch(TRANSFERMODULE fmodule, const std::string& str);
    virtual void OnTypePush(TRANSFERMODULE fmodule, const std::string& str);
        
// from RTObserverConnection
    virtual void ConnectionDisconnected();
        
private:
    void GenericMsgId(std::string& strId);
    int GenericTransSeq();
    void EstablishAck();
    void OnEstablishConn();
    void OnEstablishAck();
private:
    char			*m_pBuffer;
    int				m_nBufLen;
    int				m_nBufOffset;
    std::string     m_transferSessId;
    RTDispatchConnection  m_dispatchConnection;
    
};

#endif /* defined(__dyncRTConnector__RTTransferSession__) */
