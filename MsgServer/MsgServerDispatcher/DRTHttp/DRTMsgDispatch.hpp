//
//  DRTMsgDispatch.hpp
//  MsgServer
//
//  Created by hp on 2/18/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#ifndef DRTMsgDispatch_hpp
#define DRTMsgDispatch_hpp

#include <stdio.h>
#include "RTDispatch.h"

#define DEF_PROTO 1
#include "MsgServer/MSCommon/MSProtocol/proto/msg_type.pb.h"
#include "MsgServer/MSCommon/MSProtocol/proto/meet_msg.pb.h"
#include "MsgServer/MSCommon/MSProtocol/proto/sys_msg.pb.h"

class DRTMsgDispatch
    :public RTDispatch{
public:
    DRTMsgDispatch();
    virtual ~DRTMsgDispatch();

public:
    //for RTDispatch
    virtual void OnRecvEvent(const char*pData, int nLen) {}
    virtual void OnSendEvent(const char*pData, int nLen);
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen);
    virtual void OnTickEvent(const char*pData, int nLen) {}
};

#endif /* DRTMsgDispatch_hpp */
