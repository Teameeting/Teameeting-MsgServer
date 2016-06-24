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
#include "rtklog.h"
#include "OSMutex.h"

#define DEF_PROTO 1
#include "../../ProtoCommon.h"

class RTTransfer{
public:
    RTTransfer();
    virtual ~RTTransfer();

    int DoProcessData(const char* pData, int nLen);

public:
    virtual void OnTransfer(const std::string& str) = 0;
    virtual void OnMsgAck(pms::TransferMsg& tmsg) = 0;
    virtual void OnTypeConn(const std::string& str) = 0;
    virtual void OnTypeTrans(const std::string& str) = 0;
    virtual void OnTypeQueue(const std::string& str) = 0;
    virtual void OnTypeDispatch(const std::string& str) = 0;
    virtual void OnTypePush(const std::string& str) = 0;
    virtual void OnTypeTLogin(const std::string& str) = 0;
    virtual void OnTypeTLogout(const std::string& str) = 0;

private:

};

#endif /* defined(__MsgServer__RTTransfer__) */
