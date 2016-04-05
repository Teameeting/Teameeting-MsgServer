#include "RTDispatch.h"
#include "RTJSBuffer.h"
#include "atomic.h"


RTDispatch::RTDispatch()
: Task()
, fTimeoutTask(NULL, 120 * 1000)
, fTickTime(0)
{
	ListZero(&m_listRecv);
	ListZero(&m_listSend);
	ListZero(&m_listWakeup);
	ListZero(&m_listPush);
	fTimeoutTask.SetTask(this);
}

RTDispatch::~RTDispatch(void)
{
	ListEmpty(&m_listPush);
	ListEmpty(&m_listWakeup);
	ListEmpty(&m_listSend);
	ListEmpty(&m_listRecv);
}

int RTDispatch::PostData(const char*pData, int nLen)
{
    if (nLen > DATA_MAX_LENGTH) {
        return -1;
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+1));
        memcpy(ptr, pData, nLen);
        ptr[nLen] = '\0';
        {
            OSMutexLocker locker(&mMutexRecv);
            ListAppend(&m_listRecv, ptr, nLen);
        }
    }

	this->Signal(kReadEvent);
	return nLen;
}

int RTDispatch::SendData(const char*pData, int nLen)
{
    if (nLen > DATA_MAX_LENGTH) {
        return -1;
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+1));
        memcpy(ptr, pData, nLen);
        ptr[nLen] = '\0';
        {
            OSMutexLocker locker(&mMutexSend);
            ListAppend(&m_listSend, ptr, nLen);
        }
    }

	this->Signal(kWriteEvent);
	return nLen;
}

int RTDispatch::WakeupData(const char*pData, int nLen)
{
    if (nLen > DATA_MAX_LENGTH) {
        return -1;
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+1));
        memcpy(ptr, pData, nLen);
        ptr[nLen] = '\0';
        {
            OSMutexLocker locker(&mMutexWakeup);
            ListAppend(&m_listWakeup, ptr, nLen);
        }
    }

    this->Signal(kWakeupEvent);
    return nLen;
}

int RTDispatch::PushData(const char*pData, int nLen)
{
    if (nLen > DATA_MAX_LENGTH) {
        return -1;
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+1));
        memcpy(ptr, pData, nLen);
        ptr[nLen] = '\0';
        {
            OSMutexLocker locker(&mMutexPush);
            ListAppend(&m_listPush, ptr, nLen);
        }
    }

    this->Signal(kPushEvent);
    return nLen;
}

SInt64 RTDispatch::Run()
{
	EventFlags events = this->GetEvents();
	this->ForceSameThread();

	// Http session is short connection, need to kill session when occur TimeoutEvent.
	// So return -1.
    if (events&Task::kTimeoutEvent) {
        UpdateTimer();
		return 0;
    }
	if(events&Task::kKillEvent)
	{
        return -1;
	}

	while(1)
	{
		if(events&Task::kReadEvent)
		{
            ListElement *elem = NULL;
            if((elem = m_listRecv.first) != NULL)
            {
                OnRecvEvent((char*)elem->content, elem->size);
                {
                    OSMutexLocker locker(&mMutexRecv);
                    ListRemoveHead(&m_listRecv);
                }
                if(NULL != m_listRecv.first)
                    this->Signal(kReadEvent);
            }
			events -= Task::kReadEvent;
		}
		else if(events&Task::kWriteEvent)
		{
            ListElement *elem = NULL;
            if((elem = m_listSend.first) != NULL)
            {
                OnSendEvent((char*)elem->content, elem->size);
                {
                    OSMutexLocker locker(&mMutexSend);
                    ListRemoveHead(&m_listSend);
                }
                if(NULL != m_listSend.first)
                    this->Signal(kWriteEvent);
            }
			events -= Task::kWriteEvent;
		}
		else if(events&Task::kWakeupEvent)
		{
            ListElement *elem = NULL;
            if((elem = m_listWakeup.first) != NULL)
            {
                OnWakeupEvent((char*)elem->content, elem->size);
                {
                    OSMutexLocker locker(&mMutexWakeup);
                    ListRemoveHead(&m_listWakeup);
                }
                if(NULL != m_listWakeup.first)
                    this->Signal(kWakeupEvent);
            }
			events -= Task::kWakeupEvent;
		}
		else if(events&Task::kPushEvent)
		{
            ListElement *elem = NULL;
            if((elem = m_listPush.first) != NULL)
            {
                OnPushEvent((char*)elem->content, elem->size);
                {
                    OSMutexLocker locker(&mMutexPush);
                    ListRemoveHead(&m_listPush);
                }
                if(NULL != m_listPush.first)
                    this->Signal(kPushEvent);
            }
			events -= Task::kPushEvent;
		}
		else if(events&Task::kIdleEvent)
		{
			OnTickEvent("", 0);
			events -= Task::kIdleEvent;
		}
		else
		{
			return fTickTime;
		}
	}
    return 0;
}
