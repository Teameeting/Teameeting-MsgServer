//
//  LRTTransferSession.h
//  MsgServerLogical
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerLogical__LRTTransferSession__
#define __MsgServerLogical__LRTTransferSession__

#include <stdio.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <queue>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcpNoTimeout.h"
#include "RTJSBuffer.h"
#include "RTLstorage.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class LRTTransferSession
    : public RTTcpNoTimeout
    , public RTJSBuffer
    , public RTLstorage
    , public RTObserverConnection{
public:
    LRTTransferSession();
    virtual ~LRTTransferSession();
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

    void PushReadMsg(pms::StorageMsg store);
    void PushWriteMsg(pms::StorageMsg store);

public:
    void EstablishConnection();

// from RTTcpNoTimeout
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen);
    virtual void OnTickEvent(const char*pData, int nLen);

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

    pms::PackedStoreMsg             m_respReadMsg;
    std::queue<pms::StorageMsg>     m_queueReadMsg;
    OSMutex                         m_mutexQRead;

    pms::PackedStoreMsg             m_respWriteMsg;
    std::queue<pms::StorageMsg>     m_queueWriteMsg;
    OSMutex                         m_mutexQWrite;

    int                             m_isRun;

    unsigned int                    m_tmpWMsgId;
    unsigned int                    m_tmpRSeqnId;
    unsigned int                    m_tmpRDataId;
    unsigned int                    m_tmpRSeqn4DataId;
    unsigned int                    m_tmpRPDataId; // this is for pusher CGETDATA
    unsigned int                    m_tmpRPSeqn4DataId; // this is for pusher CGETDATA
    unsigned int                    m_tmpRData2Id;
};

#endif /* defined(__MsgServerLogical__LRTTransferSession__) */
