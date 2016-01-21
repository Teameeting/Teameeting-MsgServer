//
//  CRTConnTcpListener.h
//  MsgServerConnector
//
//  Created by hp on 12/11/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTConnTcpListener__
#define __MsgServerConnector__CRTConnTcpListener__

#include "TCPListenerSocket.h"

class CRTConnTcpListener
: public TCPListenerSocket
{
public:
    CRTConnTcpListener(){}
    virtual ~CRTConnTcpListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerConnector__CRTConnTcpListener__) */
