//
//  RTDispatchConnection.h
//  dyncRTConnector
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTConnector__RTDispatchConnection__
#define __dyncRTConnector__RTDispatchConnection__

#include <stdio.h>
#include <iostream>
#include "OSMutex.h"

class RTDispatchConnection{
public:
    RTDispatchConnection(){}
    virtual ~RTDispatchConnection(){}
    void DispatchMsg(const std::string& uid, const std::string& msg);
    static std::string     m_connIp;
    static std::string     m_connPort;
private:
    OSMutex                m_mutex;
};

#endif /* defined(__dyncRTConnector__RTDispatchConnection__) */
