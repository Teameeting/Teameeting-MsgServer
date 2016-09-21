#ifndef __XTCP_TICK_H__
#define __XTCP_TICK_H__
#include "webrtc/base/thread.h"

class XTcpTick
{
public:
	XTcpTick(void);
	virtual ~XTcpTick(void);

public:
	virtual int Open(void);
	virtual void Close(void);

	virtual void OnTick(void) = 0;
	virtual void OnFinish(void) = 0;

	rtc::Thread*	CurThread();
private:
	rtc::Thread		*m_pThread;
};

#endif	// __XTCP_TICK_H__

