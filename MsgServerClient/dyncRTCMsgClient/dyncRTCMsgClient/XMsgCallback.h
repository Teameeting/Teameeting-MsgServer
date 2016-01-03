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
    virtual void OnReqLogin(int code, const std::string& status, const std::string& userid) = 0;
    virtual void OnRespLogin(int code, const std::string& status, const std::string& userid) = 0;
    virtual void OnReqSndMsg(const std::string& msg) = 0;
    virtual void OnRespSndMsg(const std::string& msg) = 0;
    virtual void OnReqGetMsg(const std::string& msg) = 0;
    virtual void OnRespGetMsg(const std::string& msg) = 0;
    virtual void OnReqLogout(int code, const std::string& status, const std::string& userid) = 0;
    virtual void OnRespLogout(int code, const std::string& status, const std::string& userid) = 0;
protected:
    virtual ~XMsgCallback(){}
};

#endif
