//
//  XMsgCallback.h
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef dyncRTCMsgClient_XMsgCallback_h
#define dyncRTCMsgClient_XMsgCallback_h

class XMsgCallback {
public:
    virtual void OnLogin(int code) = 0;
    virtual void OnSndMsg(const std::string msg) = 0;
    virtual void OnGetMsg(const std::string from, const std::string msg) = 0;
    virtual void OnLogout(int code) = 0;
protected:
    virtual ~XMsgCallback(){}
};

#endif
