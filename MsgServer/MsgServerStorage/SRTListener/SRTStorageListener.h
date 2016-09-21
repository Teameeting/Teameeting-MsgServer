//
//  SRTStorageListener.h
//  MsgServerStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTStorageListener__
#define __MsgServerStorage__SRTStorageListener__

#include "TCPListenerSocket.h"

class SRTStorageListener
: public TCPListenerSocket
{
public:
    SRTStorageListener(){}
    virtual ~SRTStorageListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerStorage__SRTStorageListener__) */
