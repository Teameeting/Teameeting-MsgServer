#ifndef __RT_MEETING_H__
#define __RT_MEETING_H__
#include "TCPListenerSocket.h"
#include "RTModuleListener.h"
#include "RTTransferSession.h"
#include <iostream>

class RTMsgQueue
{
protected:
	RTMsgQueue(void);
public:
	virtual ~RTMsgQueue(void);

	/* 打印服务版本号
	 * 参数:	无
	 */
	static void PrintVersion();
	/* 系统初始化
	 * 参数:	evTbSize - 系统的Hash表大小，根据系统的设计容量而变化
	 */
	static void Initialize(int evTbSize);
	/* 系统注销
	 * 参数:	无
	 */
	static void DeInitialize();

	/*
	 *
	 */
	static RTMsgQueue* Inst();
    

public:
	int		Start(const char*pConnIp, unsigned short usConnPort, const char*pModuleIp, unsigned short usModulePort);
	void	DoTick();
	void	Stop();

private:
    RTModuleListener                *m_pModuleListener;
    RTTransferSession               *m_pTransferSession;

	OSMutex							m_MutexRoom;
	OSMutex							m_MutexSession;

};

#endif	// __RT_MEETING_H__