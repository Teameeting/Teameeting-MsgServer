//
//  LRTRTLiveListener.h
//  MsgServerRTLive
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTRTLiveListener__
#define __MsgServerRTLive__LRTRTLiveListener__

#include "TCPListenerSocket.h"

class LRTRTLiveListener
: public TCPListenerSocket
{
public:
    LRTRTLiveListener(){}
    virtual ~LRTRTLiveListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerRTLive__LRTRTLiveListener__) */
