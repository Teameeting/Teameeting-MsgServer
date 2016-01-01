//
//  RTModuleListener.h
//  dyncRTMsgQueue
//
//  Created by hp on 11/20/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMsgQueue__RTModuleListener__
#define __dyncRTMsgQueue__RTModuleListener__

#include "TcpListenerSocket.h"

class RTModuleListener
: public TCPListenerSocket
{
public:
    RTModuleListener(){};
    virtual ~RTModuleListener(void){};
    
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__dyncRTMsgQueue__RTModuleListener__) */
