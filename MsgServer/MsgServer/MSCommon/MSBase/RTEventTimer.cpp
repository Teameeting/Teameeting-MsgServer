#include "RTEventTimer.h"
#include "RTJSBuffer.h"
#include "atomic.h"


RTEventTimer::RTEventTimer(int timeout, ExecutorDelay executor)
: Task()
, mTimeout(timeout+1)
, fTimeoutTask(NULL, (timeout+1) * 1000)
, fTickTime(0)
, mExecutorDelay(executor)
{
	ListZero(&mListDelay);
	fTimeoutTask.SetTask(this);
}

RTEventTimer::~RTEventTimer(void)
{
	ListEmpty(&mListDelay);
    mExecutorDelay = NULL;
}

int RTEventTimer::DataDelay(const char*pData, int nLen)
{
    if (nLen > DATA_MAX_LENGTH) {
        return -1;
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+1));
        memcpy(ptr, pData, nLen);
        ptr[nLen] = '\0';
        {
            OSMutexLocker locker(&mMutexDelay);
            ListAppend(&mListDelay, ptr, nLen);
        }
    }

    // no event to signal, just wait to timeout
	return nLen;
}

SInt64 RTEventTimer::Run()
{
	EventFlags events = this->GetEvents();
	this->ForceSameThread();

    if(events&Task::kKillEvent)
    {
        return -1;
    }

	while(1)
	{
		if(events&Task::kTimeoutEvent)
		{
            ListElement *elem = NULL;
            if((elem = mListDelay.first) != NULL)
            {
                {
                    OSMutexLocker locker(&mMutexDelay);
                    if (mExecutorDelay) {
                        mExecutorDelay((const char*)elem->content, (int)elem->size);
                    }
                    ListRemoveHead(&mListDelay);
                }
            }
			events -= Task::kTimeoutEvent;
            return -1;
		}
		else
		{
			return 0;
		}
	}
    return 0;
}
