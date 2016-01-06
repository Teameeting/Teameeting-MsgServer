//
//  RTConnectionTcp.h
//  dyncRTConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTConnector__RTConnectionTcp__
#define __dyncRTConnector__RTConnectionTcp__

#include "RTTcp.h"
#include "RTConnTcp.h"
#include "RTObserverConnection.h"

class RTConnectionTcp
: public RTTcp
, public RTConnTcp
, public RTObserverConnection{
public:
    RTConnectionTcp();
    virtual ~RTConnectionTcp();
public:
    int SendDispatch(const std::string& id, const std::string& msg);
    void GenericResponse(SIGNALTYPE stype, MSGTYPE mtype, long long mseq, int code, const std::string& status, std::string& resp);
public:
    //* For RCTcp
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnLcsEvent();
    virtual void OnPeerEvent(){Assert(false);};
    virtual void OnTickEvent(){};
    
public:
    //* For RTConnTcp
    virtual void OnLogin(const char* pUserid, const char* pPass);
    virtual void OnSndMsg(MSGTYPE mType, long long mseq, const char* pUserid, const char* pData, int dLen);
    virtual void OnGetMsg(MSGTYPE mType, long long mseq, const char* pUserid);
    virtual void OnLogout(const char* pUserid);
    virtual void OnKeepAlive(const char* pUserid);
    virtual void OnResponse(const char*pData, int nLen);
public:
    //* For RTObserverConnection
    virtual void ConnectionDisconnected();
private:
    int GenericTransSeq();
private:
    char			*m_pBuffer;
    int				m_nBufLen;
    int				m_nBufOffset;
    std::string     m_connectorId;
    std::string     m_userId;
};

#endif /* defined(__dyncRTConnector__RTConnectionTcp__) */
