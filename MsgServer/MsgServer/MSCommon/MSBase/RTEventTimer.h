#ifndef __RT_EVENT_TIMER_H__
#define __RT_EVENT_TIMER_H__

#include <unordered_map>
#include <utility>
#include "LinkedList.h"
#include "StrPtrLen.h"
#include "Task.h"
#include "TimeoutTask.h"
#include "OSMutex.h"
#include "RTType.h"
#include "RTObserverConnection.h"

#define DATA_MAX_LENGTH (8192)
#define RETRY_MAX_TIME  (30)

class RTEventTimer
	: public Task
{
public:
    typedef int (*ExecutorDelay)(const char*pData, int nLen);
    typedef int (*ExecutorDelay2)(const void*self, const void*caller);

	RTEventTimer(int timeout, ExecutorDelay executor);
	virtual ~RTEventTimer(void);

	void SetTimer(int time){Assert(time > 0);fTimeoutTask.SetTimeout(time);};
	void SetTickTimer(int time){Assert(time > 0);fTickTime = time;};
	void UpdateTimer(){fTimeoutTask.RefreshTimeout();};

    int DataDelay(const char*pData, int nLen);
    int SetOutTime(int time) { mOutTime = time; }
    void SetExecutor2Callback(ExecutorDelay2 exec2) { mExecutorDelay2 = exec2; }
    void SetExecutor2Param(void* self, void* caller) { mExecutor2Param1 = self; mExecutor2Param2 = caller; }


protected:
	//* For Task
	virtual SInt64 Run();

    // Observer
    void AddObserver(RTObserverConnection* conn);
    void DelObserver(RTObserverConnection* conn);
private:
	TimeoutTask         fTimeoutTask;//allows the session to be timed out
	UInt32				fTickTime;
	List				mListDelay;
    OSMutex             mMutexDelay;
    ExecutorDelay       mExecutorDelay;
    int                 mTimeout;

    //this is designed for a long-live timer, default -1, use once.
    //if set to 0, it can trigger ulimit times
    int                 mOutTime;
    ExecutorDelay2      mExecutorDelay2;
    void*               mExecutor2Param1;
    void*               mExecutor2Param2;

    typedef std::unordered_map<RTEventTimer*, RTObserverConnection*> ObserverConnectionMap;
    typedef ObserverConnectionMap::iterator ObserverConnectionMapIt;
    ObserverConnectionMap m_mapConnectObserver;
    std::pair<ObserverConnectionMapIt, bool> m_OCMItPair;
};

#endif	// __RT_EVENT_TIMER_H__
