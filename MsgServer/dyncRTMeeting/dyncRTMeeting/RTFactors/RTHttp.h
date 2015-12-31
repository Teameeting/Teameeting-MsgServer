//
//  RTHttp.h
//  dyncRTMeeting
//
//  Created by hp on 12/16/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __dyncRTMeeting__RTHttp__
#define __dyncRTMeeting__RTHttp__

#include <stdio.h>
#include "Task.h"
#include "LinkedList.h"

enum
{
    kFirstHttpSessionID     = 1,    //UInt32
};

class RTHttp : public Task{
public:
    RTHttp();
    virtual ~RTHttp();
    
   	int SendData(const char*pData, int nLen);
    
    virtual void OnReadEvent(const char*pData, int nLen) = 0;
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
#endif /* defined(__dyncRTMeeting__RTHttp__) */
