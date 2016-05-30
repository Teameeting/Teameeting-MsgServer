//
//  RRTRTRedisListener.h
//  MsgServerRTRedis
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTRedis__RRTRTRedisListener__
#define __MsgServerRTRedis__RRTRTRedisListener__

#include "TCPListenerSocket.h"

class RRTRTRedisListener
: public TCPListenerSocket
{
public:
    RRTRTRedisListener(){}
    virtual ~RRTRTRedisListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerRTRedis__RRTRTRedisListener__) */
