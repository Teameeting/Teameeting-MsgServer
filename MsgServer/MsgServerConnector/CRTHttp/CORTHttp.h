//
//  CORTHttp.h
//  MsgServerMeeting
//
//  Created by hp on 12/16/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerMeeting__CORTHttp__
#define __MsgServerMeeting__CORTHttp__

#include <stdio.h>
#include "Task.h"
#include "LinkedList.h"

enum
{
    kFirstHttpSessionID     = 1,    //UInt32
};

class CORTHttp : public Task{
public:
    CORTHttp();
    virtual ~CORTHttp();
    
   	int SendData(const char*pData, int nLen);
    
    virtual void OnReadEvent(const char*data, int size) = 0;
    virtual int  OnWriteEvent(const char*pData, int nLen, int* outLen) = 0;
    virtual void OnTickEvent() = 0;
    
protected:
    virtual SInt64 Run();
    
    //CONSTANTS:
    enum
    {
        kRequestBufferSizeInBytes = 2048        //UInt32
    };
    //Each TCP session has a unique number that identifies it.
    UInt32              fSessionID;
private:
    UInt32				fTickTime;
    List				m_listSend;
    static unsigned int	 sHttpSessionIDCounter;
};
#endif /* defined(__MsgServerMeeting__CORTHttp__) */
