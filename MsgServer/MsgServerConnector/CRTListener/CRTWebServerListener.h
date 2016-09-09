//
//  CRTWebServerListener.h
//  MsgServerConnector
//
//  Created by hp on 11/20/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTWebServerListener__
#define __MsgServerConnector__CRTWebServerListener__

#include "TCPListenerSocket.h"

class CRTWebServerListener
: public TCPListenerSocket
{
public:
    CRTWebServerListener(){};
    virtual ~CRTWebServerListener(void){};

public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerConnector__CRTWebServerListener__) */
