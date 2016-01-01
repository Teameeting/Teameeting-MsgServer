//
//  RTModuleListener.h
//  dyncRTConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTConnector__RTModuleListener__
#define __dyncRTConnector__RTModuleListener__

#include "TcpListenerSocket.h"

class RTModuleListener
: public TCPListenerSocket
{
public:
    RTModuleListener(){}
    virtual ~RTModuleListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__dyncRTConnector__RTModuleListener__) */
