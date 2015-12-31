//
//  RTConnTcpListener.h
//  dyncRTConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTConnector__RTConnTcpListener__
#define __dyncRTConnector__RTConnTcpListener__

#include "TcpListenerSocket.h"

class RTConnTcpListener
: public TCPListenerSocket
{
public:
    RTConnTcpListener(){}
    virtual ~RTConnTcpListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__dyncRTConnector__RTConnTcpListener__) */
