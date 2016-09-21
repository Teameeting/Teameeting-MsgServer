//
//  SRTSequenceListener.h
//  MsgServerSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerSequence__SRTSequenceListener__
#define __MsgServerSequence__SRTSequenceListener__

#include "TCPListenerSocket.h"

class SRTSequenceListener
: public TCPListenerSocket
{
public:
    SRTSequenceListener(){}
    virtual ~SRTSequenceListener(void){}
public:
    //sole job of this object is to implement this function
    virtual Task*   GetSessionTask(int osSocket, struct sockaddr_in* addr);
};

#endif /* defined(__MsgServerSequence__SRTSequenceListener__) */
