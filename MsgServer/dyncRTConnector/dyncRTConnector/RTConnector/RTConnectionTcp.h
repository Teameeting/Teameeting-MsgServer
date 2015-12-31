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
public:
    //* For RCTcp
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnLcsEvent();
    virtual void OnPeerEvent(){Assert(false);};
    virtual void OnTickEvent(){};
    
public:
    //* For RTConnTcp
    virtual void OnLogin(const char* pUserid, const char* pPass);
    virtual void OnSndMsg(const char* pUserid, int mType, const char* pData, int dLen);
    virtual void OnGetMsg(const char* pUserid, int mType);
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
