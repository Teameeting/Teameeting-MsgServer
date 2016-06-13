//
//  RTLstorage.h
//  MsgServer
//
//  Created by hp on 12/1/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServer__RTLstorage__
#define __MsgServer__RTLstorage__

#include <stdio.h>
#include <string>
#include "RTMeetMsg.h"
#include "RTMessage.h"
#include "rtklog.h"
#include "OSMutex.h"

#define DEF_PROTO 1
#if DEF_PROTO
#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/meet_msg.pb.h"
#include "MsgServer/proto/meet_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"
#endif

class RTLstorage{
public:
    RTLstorage();
    virtual ~RTLstorage();

    int DoProcessData(const char* pData, int nLen);

public:
    virtual void OnTransfer(const std::string& str) = 0;
    virtual void OnMsgAck(pms::TransferMsg& tmsg) = 0;
    virtual void OnTypeConn(const std::string& str) = 0;
    virtual void OnTypeTrans(const std::string& str) = 0;
    virtual void OnTypeQueue(const std::string& str) = 0;
    virtual void OnTypeDispatch(const std::string& str) = 0;
    virtual void OnTypePush(const std::string& str) = 0;
    virtual void OnTypeRequest(const std::string& str) = 0;
    virtual void OnTypeResponse(const std::string& str) = 0;

private:

};

#endif /* defined(__MsgServer__RTLstorage__) */
