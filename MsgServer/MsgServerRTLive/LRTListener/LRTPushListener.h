//
//  LRTPushListener.h
//  MsgServerRTLive
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTPushListener__
#define __MsgServerRTLive__LRTPushListener__

#include "TCPListenerSocket.h"

class LRTPushListener
: public TCPListenerSocket
{
public:
    LRTPushListener(){}
    virtual ~LRTPushListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerRTLive__LRTPushListener__) */
