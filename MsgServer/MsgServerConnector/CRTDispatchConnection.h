//
//  CRTDispatchConnection.h
//  MsgServerConnector
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTDispatchConnection__
#define __MsgServerConnector__CRTDispatchConnection__

#include <stdio.h>
#include <iostream>
#include "CRTMsgPush.hpp"

class CRTDispatchConnection{
public:
    CRTDispatchConnection(){}
    virtual ~CRTDispatchConnection(){}
    
    bool IsUserLive(const std::string& uid);
    void DispatchMsg(const std::string& uid, const std::string& msg);
    void PushMsg(const std::string& uid, const std::string& msg);
    static std::string     m_connIp;
    static std::string     m_connPort;
private:
    CRTMsgPush             m_msgPush;
};

#endif /* defined(__MsgServerConnector__CRTDispatchConnection__) */
