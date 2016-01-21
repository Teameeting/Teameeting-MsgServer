//
//  CRTConnListener.h
//  MsgServerConnector
//
//  Created by hp on 11/20/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTConnListener__
#define __MsgServerConnector__CRTConnListener__

#include "TCPListenerSocket.h"

class CRTConnListener
: public TCPListenerSocket
{
public:
    CRTConnListener(){};
    virtual ~CRTConnListener(void){};

public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerConnector__CRTConnListener__) */
