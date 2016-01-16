//
//  DRTTransfer.h
//  MsgServerDispatcher
//
//  Created by hp on 12/1/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerDispatcher__DRTTransfer__
#define __MsgServerDispatcher__DRTTransfer__

#include <stdio.h>
#include "RTMessage.h"
#include "rtklog.h"
#include <string>

class DRTTransfer{
public:
    DRTTransfer();
    virtual ~DRTTransfer();
    
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
    TRANSFERMSG m_msg;
};

#endif /* defined(__MsgServerDispatcher__DRTTransfer__) */
