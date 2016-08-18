//
//  PRTPusherListener.h
//  MsgServerPusher
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerPusher__PRTPusherListener__
#define __MsgServerPusher__PRTPusherListener__

#include "TCPListenerSocket.h"

class PRTPusherListener
: public TCPListenerSocket
{
public:
    PRTPusherListener(){}
    virtual ~PRTPusherListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerPusher__PRTPusherListener__) */
