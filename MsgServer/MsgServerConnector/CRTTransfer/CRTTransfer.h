//
//  CRTTransfer.h
//  MsgServerConnector
//
//  Created by hp on 12/1/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerConnector__CRTTransfer__
#define __MsgServerConnector__CRTTransfer__

#include <stdio.h>
#include <string>
#include "RTMeetMsg.h"
#include "RTMessage.h"
#include "rtklog.h"
#include "OSMutex.h"

class CRTTransfer{
public:
    CRTTransfer();
    virtual ~CRTTransfer();

    int ProcessData(const char* pData, int nLen);

public:
    virtual void OnTransfer(const std::string& str) = 0;
    virtual void OnTypeConn(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeTrans(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeQueue(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypeDispatch(TRANSFERMODULE fmodule, const std::string& str) = 0;
    virtual void OnTypePush(TRANSFERMODULE fmodule, const std::string& str) = 0;

private:
    int DoProcessData(const char* pData, int nLen);
private:
    OSMutex     m_mutexMsg;
};

#endif /* defined(__MsgServerConnector__CRTTransfer__) */
