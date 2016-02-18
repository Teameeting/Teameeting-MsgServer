//
//  CRTMsgPush.hpp
//  MsgServer
//
//  Created by hp on 2/18/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#ifndef CRTMsgPush_hpp
#define CRTMsgPush_hpp

#include <stdio.h>
#include "RTPush.h"

class CRTMsgPush
    :public RTPush{
public:
    CRTMsgPush();
    virtual ~CRTMsgPush();

public:
    //for RTPush
    virtual void OnRecvData(const char*pData, int nLen) {}
    virtual void OnWakeupEvent() {}
    virtual void OnPushEvent(const char*pData, int nLen);
    virtual void OnTickEvent() {}
};

#endif /* CRTMsgPush_hpp */
