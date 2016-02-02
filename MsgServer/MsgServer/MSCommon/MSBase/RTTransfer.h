//
//  RTTransfer.h
//  MsgServer
//
//  Created by hp on 12/1/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServer__RTTransfer__
#define __MsgServer__RTTransfer__

#include <stdio.h>
#include <string>
#include "RTMeetMsg.h"
#include "RTMessage.h"
#include "rtklog.h"
#include "OSMutex.h"

class RTTransfer{
public:
    RTTransfer();
    virtual ~RTTransfer();
    
    int DoProcessData(const char* pData, int nLen);

public:
    virtual void OnTransfer(const std::string& str) = 0;
    virtual void OnMsgAck(TRANSFERMSG& tmsg) = 0;
    virtual void OnTypeConn(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeTrans(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeQueue(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeDispatch(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypePush(TRANSFERMODULE fmodule, const std::string& str) = 0;

private:

};

#endif /* defined(__MsgServer__RTTransfer__) */
