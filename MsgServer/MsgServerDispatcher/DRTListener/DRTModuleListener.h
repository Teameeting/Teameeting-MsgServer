//
//  DRTModuleListener.h
//  MsgServerDispatcher
//
//  Created by hp on 11/20/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerDispatcher__DRTModuleListener__
#define __MsgServerDispatcher__DRTModuleListener__

#include "TCPListenerSocket.h"

class DRTModuleListener
: public TCPListenerSocket
{
public:
    DRTModuleListener(){};
    virtual ~DRTModuleListener(void){};

public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerDispatcher__DRTModuleListener__) */
