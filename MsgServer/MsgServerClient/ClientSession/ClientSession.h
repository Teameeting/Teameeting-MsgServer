//
//  ClientSession.h
//  SeqnClient
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __SeqnClient__ClientSession__
#define __SeqnClient__ClientSession__

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

class ClientSession
    : public RTTcp
    , public RTJSBuffer
    , public RTObserverConnection{
public:
    ClientSession();
    virtual ~ClientSession();
    void Init();
    void Unit();
    bool Connect(const std::string addr, int port);
    void Disconn();

    void SendTransferData(const char* pData, int nLen);
    void SendTransferData(const std::string& data);
    void UpdateTime();

public:

// from RTTcp
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen) {}

// from RTObserverConnection
    virtual void ConnectionDisconnected();
protected:
    virtual void OnRecvMessage(const char*message, int nLen);
private:
    long long   mRecvResponse;
    long long   mSendRequest;
};

#endif /* defined(__SeqnClient__ClientSession__) */
