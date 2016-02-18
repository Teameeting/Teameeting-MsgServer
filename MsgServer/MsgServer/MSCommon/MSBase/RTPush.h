#ifndef __RT_PUSH_H__
#define __RT_PUSH_H__

#include "LinkedList.h"
#include "StrPtrLen.h"
#include "Task.h"
#include "TimeoutTask.h"

class RTPush
	: public Task
{
public:
	RTPush(void);
	virtual ~RTPush(void);
	
	int SendData(const char*pData, int nLen);
	int PushData(const char*pData, int nLen);
	void SetTimer(int time){Assert(time > 0);fTimeoutTask.SetTimeout(time);};
	void SetTickTimer(int time){Assert(time > 0);fTickTime = time;};
	void UpdateTimer(){fTimeoutTask.RefreshTimeout();};

	virtual void OnRecvData(const char*pData, int nLen) = 0;
	virtual void OnWakeupEvent() = 0;
	virtual void OnPushEvent(const char*pData, int nLen) = 0;
	virtual void OnTickEvent() = 0;

protected:
	//* For Task
	virtual SInt64 Run();
    
private:
	TimeoutTask         fTimeoutTask;//allows the session to be timed out
	UInt32				fTickTime;
	List				m_listSend;
	List				m_listPush;
};

#endif	// __RT_PUSH_H__