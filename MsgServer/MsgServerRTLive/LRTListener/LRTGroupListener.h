//
//  LRTGroupListener.h
//  MsgServerRTLive
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTGroupListener__
#define __MsgServerRTLive__LRTGroupListener__

#include "TCPListenerSocket.h"

class LRTGroupListener
: public TCPListenerSocket
{
public:
    LRTGroupListener(){}
    virtual ~LRTGroupListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerRTLive__LRTGroupListener__) */
