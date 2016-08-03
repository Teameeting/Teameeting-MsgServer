//
//  GRTTransferSession.h
//  MsgServerGrouper
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerGrouper__GRTTransferSession__
#define __MsgServerGrouper__GRTTransferSession__

#include <stdio.h>
#include <iostream>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcpNoTimeout.h"
#include "RTJSBuffer.h"
#include "RTTransfer.h"
#include "RTObserverConnection.h"
#include "XGrpMsgProcesser.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class GRTTransferSession
    : public RTTcpNoTimeout
    , public RTJSBuffer
    , public RTTransfer
    , public XGrpMsgClientHelper
    , public RTObserverConnection{
public:
    GRTTransferSession();
    virtual ~GRTTransferSession();
    void Init();
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
public:
    void EstablishConnection();
    int CreateGroupSeqn(const std::string& cltUserid, const std::string& groupid);
    int DeleteGroupSeqn(const std::string& cltUserid, const std::string& groupid);
    int GenGrpSyncDataNotify(const std::string& userid, const std::string& groupid, int64 seqn);
    int GenGrpSyncDataNotifys(const std::vector<std::string>& userids, const std::string& groupid, int64 seqn);

// from RTTcpNoTimeout
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen);
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

// from XGrpMsgClientHelper
public:
    virtual void OnGroupNotify(int code, const std::string& cont);
    virtual void OnCreateGroupSeqn(int code, const std::string& cont);
    virtual void OnDeleteGroupSeqn(int code, const std::string& cont);

protected:
   virtual void OnRecvMessage(const char*message, int nLen);
// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();

private:
    std::string     m_transferSessId;
    UInt64          m_lastUpdateTime;
    std::string     m_uid;
    std::string     m_moduleId;
    std::string     m_addr;
    int             m_port;
    int             m_connectingStatus;
    XGrpMsgProcesser    *m_pGrpMsgProcesser;
    pms::EModuleType     m_module;
};

#endif /* defined(__MsgServerGrouper__GRTTransferSession__) */
