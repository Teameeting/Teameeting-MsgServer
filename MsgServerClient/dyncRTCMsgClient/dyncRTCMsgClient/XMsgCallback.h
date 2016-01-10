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

class XMsgCallback {
public:
    virtual void OnSndMsg(const std::string& msg) = 0;
    virtual void OnGetMsg(const std::string& msg) = 0;
    
public:
    virtual void OnMsgServerConnected() = 0;
    virtual void OnMsgServerDisconnect() = 0;
    virtual void OnMsgServerConnectionFailure() = 0;
protected:
    virtual ~XMsgCallback(){}
};

#endif
