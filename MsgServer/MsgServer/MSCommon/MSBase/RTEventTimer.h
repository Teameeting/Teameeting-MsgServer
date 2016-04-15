#ifndef __RT_EVENT_TIMER_H__
#define __RT_EVENT_TIMER_H__

#include "LinkedList.h"
#include "StrPtrLen.h"
#include "Task.h"
#include "TimeoutTask.h"
#include "OSMutex.h"
#include "RTType.h"

#define DATA_MAX_LENGTH (8192)
#define RETRY_MAX_TIME  (30)

class RTEventTimer
	: public Task
{
public:
    typedef int (*ExecutorDelay)(const char*pData, int nLen);
    
	RTEventTimer(int timeout, ExecutorDelay executor);
	virtual ~RTEventTimer(void);
	
	void SetTimer(int time){Assert(time > 0);fTimeoutTask.SetTimeout(time);};
	void SetTickTimer(int time){Assert(time > 0);fTickTime = time;};
	void UpdateTimer(){fTimeoutTask.RefreshTimeout();};
    
    int DataDelay(const char*pData, int nLen);

protected:
	//* For Task
	virtual SInt64 Run();
    
private:
	TimeoutTask         fTimeoutTask;//allows the session to be timed out
	UInt32				fTickTime;
	List				mListDelay;
    OSMutex             mMutexDelay;
    ExecutorDelay       mExecutorDelay;
    int                 mTimeout;

};

#endif	// __RT_EVENT_TIMER_H__