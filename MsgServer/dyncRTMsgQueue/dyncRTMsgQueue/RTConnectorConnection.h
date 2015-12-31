//
//  RTConnectorConnection.h
//  dyncRTMsgQueue
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMsgQueue__RTConnectorConnection__
#define __dyncRTMsgQueue__RTConnectorConnection__

#include <stdio.h>
#include <iostream>

class RTConnectorConnection{
public:
    RTConnectorConnection(){}
    virtual ~RTConnectorConnection(){}
    void SendToConnector(const std::string& sid, const std::string& uid, const std::string& msg);
    static std::string     m_msgQueueIp;
    static std::string     m_msgQueuePort;
private:
};

#endif /* defined(__dyncRTMsgQueue__RTConnectorConnection__) */
