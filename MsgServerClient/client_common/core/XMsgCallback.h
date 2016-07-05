//
//  XMsgCallback.h
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef dyncRTCMsgClient_XMsgCallback_h
#define dyncRTCMsgClient_XMsgCallback_h

#include <string>
#include "RTMsgCommon.h"

enum MSState{
    MSNOT_CONNECTED,
    MSCONNECTTING,
    MSCONNECTED
};

typedef struct MSCbData{
    int type;
    std::string data;
    int64       seqn;
}MSCbData;

class XMsgCallback {
public:
    virtual void OnSndMsg(int code, const std::string& msgid) = 0;
    virtual void OnCmdGroup(int code, int cmd, const std::string& groupid, const MSCbData& data) = 0;
    virtual void OnRecvMsg(int64 seqn, const std::string& msg) = 0;
    virtual void OnRecvGroupMsg(int64 seqn, const std::string& seqnid, const std::string& msg) = 0;
    
    virtual void OnSyncSeqn(int64 seqn, int role) = 0;
    virtual void OnSyncGroupSeqn(const std::string& groupid, int64 seqn) = 0;
    
    virtual void OnGroupNotify(int code, const std::string& seqnid) = 0;
    virtual void OnNotifySeqn(int code, const std::string& seqnid) = 0;
    virtual void OnNotifyData(int code, const std::string& seqnid) = 0;
    
public:
    virtual void OnMsgServerConnected() = 0;
    virtual void OnMsgServerConnecting() = 0;
    virtual void OnMsgServerDisconnect() = 0;
    virtual void OnMsgServerConnectionFailure() = 0;
    
public:
    XMsgCallback(){}
    virtual ~XMsgCallback(){}
};

#endif
