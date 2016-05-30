#ifndef __RT_EVENT_LOOPER_H__
#define __RT_EVENT_LOOPER_H__

#include "LinkedList.h"
#include "StrPtrLen.h"
#include "Task.h"
#include "TimeoutTask.h"
#include "OSMutex.h"
#include "RTType.h"

class RTEventLooper
	: public Task
{
public:
	RTEventLooper(void);
	virtual ~RTEventLooper(void);

	void SetTimer(int time){Assert(time > 0);fTimeoutTask.SetTimeout(time);};
	void SetTickTimer(int time){Assert(time > 0);fTickTime = time;};
	void UpdateTimer(){fTimeoutTask.RefreshTimeout();};

	int PostData(const char*pData, int nSize);
	int SendData(const void*pData, int nSize);
	int WakeupData(const char*pData, int nSize);
	int PushData(const char*pData, int nSize);

	virtual void OnPostEvent(const char*pData, int nSize) = 0;
	virtual void OnSendEvent(const void*pData, int nSize) = 0;
	virtual void OnWakeupEvent(const char*pData, int nSize) = 0;
	virtual void OnPushEvent(const char*pData, int nSize) = 0;
	virtual void OnTickEvent(const void*pData, int nSize) = 0;

protected:
	//* For Task
	virtual SInt64 Run();

private:
	TimeoutTask         fTimeoutTask;//allows the session to be timed out
	UInt32				fTickTime;

	List				m_listRecv;
	List				m_listSend;
	List				m_listWakeup;
	List				m_listPush;

    OSMutex             mMutexRecv;
    OSMutex             mMutexSend;
    OSMutex             mMutexWakeup;
    OSMutex             mMutexPush;
};

#endif	// __RT_EVENT_LOOPER_H__
