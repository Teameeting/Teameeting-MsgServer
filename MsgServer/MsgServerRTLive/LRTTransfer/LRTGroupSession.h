//
//  LRTGroupSession.h
//  MsgServerRTLive
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTGroupSession__
#define __MsgServerRTLive__LRTGroupSession__

#include <stdio.h>
#include <iostream>
#include <queue>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcp.h"
#include "RTJSBuffer.h"
#include "LRTGrpConnTcp.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class LRTGroupSession
    : public RTTcp
    , public LRTGrpConnTcp
    , public RTJSBuffer
    , public RTObserverConnection{
public:
    LRTGroupSession();
    virtual ~LRTGroupSession();
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
    void GenericResponse(pms::EServerCmd cmd, pms::EModuleType module, int code, std::string& resp);

public:
    void EstablishConnection();

    void PushNewMsg2Queue(const std::string& str)
    {
        {
            OSMutexLocker locker(&m_mutexQueueNew);
            m_queueNewMsg.push(str);
        }
        this->Signal(Task::kPushEvent);
    }

    void PushSeqnReq2Queue(const std::string& str)
    {
        {
            OSMutexLocker locker(&m_mutexQueueSeqn);
            m_queueSeqnMsg.push(str);
        }
        this->Signal(Task::kWakeupEvent);
    }

    void PushDataReq2Queue(const std::string& str)
    {
        {
            OSMutexLocker locker(&m_mutexQueueData);
            m_queueDataMsg.push(str);
        }
        this->Signal(Task::kIdleEvent);
    }

// from RTTcpNoTimeout
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen);
    virtual void OnPushEvent(const char*pData, int nLen);
    virtual void OnTickEvent(const char*pData, int nLen);

// from LRTGrpConnTcp
public:

    virtual void OnLogin(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnSndMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnGetMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnLogout(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnKeepAlive(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnSyncSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnSyncData(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnGroupNotify(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);

    virtual void OnResponse(const char*pData, int nLen);

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

    pms::PackedStoreMsg         m_packedNewMsg;
    std::queue<std::string>     m_queueNewMsg;
    OSMutex                     m_mutexQueueNew;

    pms::PackedStoreMsg         m_packedSeqnMsg;
    std::queue<std::string>     m_queueSeqnMsg;
    OSMutex                     m_mutexQueueSeqn;

    pms::PackedStoreMsg         m_packedDataMsg;
    std::queue<std::string>     m_queueDataMsg;
    OSMutex                     m_mutexQueueData;

    unsigned int                m_wNewMsgId;
    std::string                 m_userid;
    std::string                 m_token;
    bool                        m_login;
};

#endif /* defined(__MsgServerRTLive__LRTGroupSession__) */
