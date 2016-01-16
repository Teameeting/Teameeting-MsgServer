//
//  CRTModuleListener.h
//  MsgServerConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTModuleListener__
#define __MsgServerConnector__CRTModuleListener__

#include "TcpListenerSocket.h"

class CRTModuleListener
: public TCPListenerSocket
{
public:
    CRTModuleListener(){}
    virtual ~CRTModuleListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerConnector__CRTModuleListener__) */
