//
//  PRTConnDispatcher.h
//  MsgServerPusher
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerPusher__PRTConnDispatcher__
#define __MsgServerPusher__PRTConnDispatcher__

#include <stdio.h>
#include "RTDispatch.h"
#include "RTObserverConnection.h"

class PRTConnDispatcher : public RTDispatch, public RTObserverConnection{
public:
    PRTConnDispatcher(): RTDispatch(){ AddObserver(this); }
    ~PRTConnDispatcher(){ DelObserver(this); }

    // for RTDispatch
    virtual void OnRecvEvent(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen);

// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();
private:

};

#endif /* defined(__MsgServerPusher__PRTConnDispatcher__) */
