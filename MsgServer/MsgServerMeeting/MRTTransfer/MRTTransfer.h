//
//  MRTTransfer.h
//  MsgServerMeeting
//
//  Created by hp on 12/1/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerMeeting__MRTTransfer__
#define __MsgServerMeeting__MRTTransfer__

#include <stdio.h>
#include "RTMeetMsg.h"
#include "RTMessage.h"
#include "rtklog.h"
#include <string>

class MRTTransfer{
public:
    MRTTransfer();
    virtual ~MRTTransfer();

    int DoProcessData(const char* pData, int nLen);
public:
    virtual void OnTransfer(const std::string& str) = 0;
    virtual void OnTypeConn(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeTrans(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeQueue(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeDispatch(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypePush(TRANSFERMODULE fmodule, const std::string& str) = 0;

private:

};

#endif /* defined(__MsgServerMeeting__MRTTransfer__) */
