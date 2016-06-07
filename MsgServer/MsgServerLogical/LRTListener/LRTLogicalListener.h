//
//  LRTLogicalListener.h
//  MsgServerLogical
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerLogical__LRTLogicalListener__
#define __MsgServerLogical__LRTLogicalListener__

#include "TCPListenerSocket.h"

class LRTLogicalListener
: public TCPListenerSocket
{
public:
    LRTLogicalListener(){}
    virtual ~LRTLogicalListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerLogical__LRTLogicalListener__) */
