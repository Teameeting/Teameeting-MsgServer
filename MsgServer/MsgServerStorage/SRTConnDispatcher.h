//
//  SRTConnDispatcher.h
//  MsgServerStorage
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTConnDispatcher__
#define __MsgServerStorage__SRTConnDispatcher__

#include <stdio.h>
#include "RTDispatch.h"
#include "RTObserverConnection.h"

class SRTConnDispatcher : public RTDispatch, public RTObserverConnection{
public:
    SRTConnDispatcher(): RTDispatch(){ AddObserver(this); }
    ~SRTConnDispatcher(){ DelObserver(this); }

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

#endif /* defined(__MsgServerStorage__SRTConnDispatcher__) */
