//
//  DRTConnDispatcher.h
//  MsgServerDispatcher
//
//  Created by hp on 12/9/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerDispatcher__DRTConnDispatcher__
#define __MsgServerDispatcher__DRTConnDispatcher__

#include <stdio.h>
#include "RTDispatch.h"

class DRTConnDispatcher : public RTDispatch{
public:
    DRTConnDispatcher(): RTDispatch(){}
    ~DRTConnDispatcher(){}

    // for RTDispatch
    virtual void OnRecvEvent(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen);
private:

};

#endif /* defined(__MsgServerDispatcher__DRTConnDispatcher__) */
