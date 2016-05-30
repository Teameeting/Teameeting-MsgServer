#include "RTEventLooper.h"
#include "atomic.h"


RTEventLooper::RTEventLooper()
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

RTEventLooper::~RTEventLooper(void)
{
	ListEmpty(&m_listPush);
	ListEmpty(&m_listWakeup);
	ListEmpty(&m_listSend);
	ListEmpty(&m_listRecv);
}

int RTEventLooper::PostData(const char*pData, int nSize)
{
    {
        char* ptr = (char*)malloc(sizeof(char)*(nSize+1));
        memcpy(ptr, pData, nSize);
        ptr[nSize] = '\0';
        {
            OSMutexLocker locker(&mMutexRecv);
            ListAppend(&m_listRecv, ptr, nSize);
        }
    }

	this->Signal(kReadEvent);
	return nSize;
}

int RTEventLooper::SendData(const void*pData, int nSize)
{
    {
        void* ptr = (void*)malloc(sizeof(char)*(nSize));
        memcpy(ptr, pData, nSize);
        {
            OSMutexLocker locker(&mMutexSend);
            ListAppend(&m_listSend, ptr, nSize);
        }
    }

	this->Signal(kWriteEvent);
	return nSize;
}

int RTEventLooper::WakeupData(const char*pData, int nSize)
{
    {
        char* ptr = (char*)malloc(sizeof(char)*(nSize+1));
        memcpy(ptr, pData, nSize);
        ptr[nSize] = '\0';
        {
            OSMutexLocker locker(&mMutexWakeup);
            ListAppend(&m_listWakeup, ptr, nSize);
        }
    }

    this->Signal(kWakeupEvent);
    return nSize;
}

int RTEventLooper::PushData(const char*pData, int nSize)
{
    {
        char* ptr = (char*)malloc(sizeof(char)*(nSize+1));
        memcpy(ptr, pData, nSize);
        ptr[nSize] = '\0';
        {
            OSMutexLocker locker(&mMutexPush);
            ListAppend(&m_listPush, ptr, nSize);
        }
    }

    this->Signal(kPushEvent);
    return nSize;
}

SInt64 RTEventLooper::Run()
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
                OnPostEvent((const char*)elem->content, elem->size);
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
                OnSendEvent(elem->content, elem->size);
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
                OnWakeupEvent((const char*)elem->content, elem->size);
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
                OnPushEvent((const char*)elem->content, elem->size);
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
			OnTickEvent(NULL, 0);
			events -= Task::kIdleEvent;
		}
		else
		{
			return fTickTime;
		}
	}
    return 0;
}
