//
//  LRTConnDispatcher.h
//  MsgServerRTLive
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTConnDispatcher__
#define __MsgServerRTLive__LRTConnDispatcher__

#include <stdio.h>
#include "RTDispatch.h"

class LRTConnDispatcher : public RTDispatch{
public:
    LRTConnDispatcher(): RTDispatch(){}
    ~LRTConnDispatcher(){}

    // for RTDispatch
    virtual void OnRecvEvent(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen);
private:

};

#endif /* defined(__MsgServerRTLive__LRTConnDispatcher__) */
