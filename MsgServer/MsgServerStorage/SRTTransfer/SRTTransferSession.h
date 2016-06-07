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
#include "RTTransfer.h"
#include "RTObserverConnection.h"
#include "SRTRedisManager.h"

#define DEF_PROTO 1

class SRTTransferSession
    : public RTTcpNoTimeout
    , public RTJSBuffer
    , public RTObserverConnection{
public:
    SRTTransferSession();
    virtual ~SRTTransferSession();
    void Init();
    void InitConf();
    void Unit();
    bool Connect(const std::string addr, int port);
    void Disconn();

    void SendTransferData(const char* pData, int nLen);
    void SendTransferData(const std::string& data);

    void RefreshTime();

    // from RTTcpNoTimeout
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen) { printf("OnTickEvent\n");}

// from RTObserverConnection
    virtual void ConnectionDisconnected();
protected:
    virtual void OnRecvMessage(const char*message, int nLen);
private:
    long long       mRecvRequest;
    List            m_listRequest;
    OSMutex         m_mutexList;
    SRTRedisManager                     m_RedisManager;

};

#endif /* defined(__MsgServerStorage__SRTTransferSession__) */
