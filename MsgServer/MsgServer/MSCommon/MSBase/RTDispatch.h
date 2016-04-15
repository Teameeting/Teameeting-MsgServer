#ifndef __RT_DISPATCH_H__
#define __RT_DISPATCH_H__

#include "LinkedList.h"
#include "StrPtrLen.h"
#include "Task.h"
#include "TimeoutTask.h"
#include "OSMutex.h"
#include "RTType.h"

#define DATA_MAX_LENGTH (8192)

class RTDispatch
	: public Task
{
public:
	RTDispatch(void);
	virtual ~RTDispatch(void);
    
	void SetTimer(int time){Assert(time > 0);fTimeoutTask.SetTimeout(time);};
	void SetTickTimer(int time){Assert(time > 0);fTickTime = time;};
	void UpdateTimer(){fTimeoutTask.RefreshTimeout();};
    
	int PostData(const char*pData, int nLen);
	int SendData(const char*pData, int nLen);
	int WakeupData(const char*pData, int nLen);
	int PushData(const char*pData, int nLen);

	virtual void OnRecvEvent(const char*pData, int nLen) = 0;
	virtual void OnSendEvent(const char*pData, int nLen) = 0;
	virtual void OnWakeupEvent(const char*pData, int nLen) = 0;
	virtual void OnPushEvent(const char*pData, int nLen) = 0;
	virtual void OnTickEvent(const char*pData, int nLen) = 0;

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

#endif	// __RT_DISPATCH_H__