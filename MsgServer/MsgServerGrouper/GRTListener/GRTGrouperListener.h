//
//  GRTGrouperListener.h
//  MsgServerGrouper
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerGrouper__GRTGrouperListener__
#define __MsgServerGrouper__GRTGrouperListener__

#include "TCPListenerSocket.h"

class GRTGrouperListener
: public TCPListenerSocket
{
public:
    GRTGrouperListener(){}
    virtual ~GRTGrouperListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerGrouper__GRTGrouperListener__) */
