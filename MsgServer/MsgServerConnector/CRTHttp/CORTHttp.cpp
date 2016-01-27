//
//  CORTHttp.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/16/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CORTHttp.h"
#include "atomic.h"

unsigned int CORTHttp::sHttpSessionIDCounter = kFirstHttpSessionID;

CORTHttp::CORTHttp()
: Task()
, fTickTime(0)
{
    ListZero(&m_listSend);
    fSessionID = (UInt32)atomic_add(&sHttpSessionIDCounter, 1);
}


CORTHttp::~CORTHttp()
{
    ListEmpty(&m_listSend);
}

int CORTHttp::SendData(const char*pData, int nLen)
{
    if (nLen > 9999) {
        LE("%s invalid params error!\n", __FUNCTION__);
        return -1;
    }
    {
        char* ptr = new char[nLen];
        memcpy(ptr, pData, nLen);
        ListAppend(&m_listSend, ptr, nLen);
    }
    LI("CORTHttp::SendData ok\n");
    this->Signal(kWriteEvent);
    return nLen;
}

SInt64 CORTHttp::Run()
{
    EventFlags events = this->GetEvents();
    this->ForceSameThread();
    
    // Http session is short connection, need to kill session when occur TimeoutEvent.
    // So return -1.
    if(events&Task::kKillEvent)
    {
        LE("killEvent\n");
        return -1;
    }
    
    while(true)
    {
        if(events&Task::kReadEvent)
        {
            OnReadEvent(NULL, 0);
            events -= Task::kReadEvent;
        }
        else if(events&Task::kWriteEvent)
        {
            ListElement *elem = NULL;
            LI("NOTIFICATION OnWriteEvent \n");
            if((elem = m_listSend.first) != NULL)
            {
                int theLengthSent = 0;
                LI("NOTIFICATION OnWriteEvent MSG:%s\n", (char*)elem->content);
                int err = OnWriteEvent((char*)elem->content, elem->size, &theLengthSent);
                if (!err)
                {
                    ListRemoveHead(&m_listSend);
                    if(NULL != m_listSend.first) {
                        this->Signal(kWriteEvent);
                    }
                }
                else
                {
                    LE("send error");
                }
            } else {
                LE("NOTIFICATION OnWriteEvent error\n");
            }
            events -= Task::kWriteEvent;
        }
        else if(events&Task::kIdleEvent)
        {
            OnTickEvent();
            events -= Task::kIdleEvent; 
        }
        else
        {
            return fTickTime;
        }
    }
    
    // If we are here because of a timeout, but we can't delete because someone
    // is holding onto a reference to this session, just reschedule the timeout.
    //
    // At this point, however, the session is DEAD.
    return 0;
}
