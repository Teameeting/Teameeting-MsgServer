//
//  RTConnListener.h
//  dyncRTConnector
//
//  Created by hp on 11/20/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMsg__RTConnListener__
#define __dyncRTMsg__RTConnListener__

#include "TcpListenerSocket.h"

class RTConnListener
: public TCPListenerSocket
{
public:
    RTConnListener(){};
    virtual ~RTConnListener(void){};
    
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__dyncRTMsg__RTConnListener__) */
