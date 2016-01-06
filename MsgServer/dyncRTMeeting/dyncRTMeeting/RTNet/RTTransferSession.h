//
//  RTTransferSession.h
//  dyncRTMeeting
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMeeting__RTTransferSession__
#define __dyncRTMeeting__RTTransferSession__

#include <stdio.h>
#include <iostream>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcp.h"
#include "RTTransfer.h"
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
    bool RefreshTime();
    void KeepAlive();
    
    void SendTransferData(const char* pData, int nLen);
    void SetModuleId(std::string& moduleId) { m_moduleId = moduleId; }
    void TestConnection();
        
public:
    void EstablishConnection();
    void OnConnectionLostNotify(const std::string& uid);

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
public:
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
    UInt64          m_lastUpdateTime;
    std::string     m_moduleId;
};

#endif /* defined(__dyncRTMeeting__RTTransferSession__) */
