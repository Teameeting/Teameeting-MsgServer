//
//  MRTModuleListener.h
//  MsgServerModule
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerModule__MRTModuleListener__
#define __MsgServerModule__MRTModuleListener__

#include "TCPListenerSocket.h"

class MRTModuleListener
: public TCPListenerSocket
{
public:
    MRTModuleListener(){}
    virtual ~MRTModuleListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerModule__MRTModuleListener__) */
