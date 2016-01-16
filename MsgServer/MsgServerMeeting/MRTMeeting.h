#ifndef __MsgServerMeeting_MRT_MEETING_H__
#define __MsgServerMeeting_MRT_MEETING_H__
#include "TCPListenerSocket.h"
#include "MRTTransferSession.h"
#include <iostream>


class MRTMeeting
{
protected:
	MRTMeeting(void);
public:
	virtual ~MRTMeeting(void);

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
	static MRTMeeting* Inst();

public:
	int		Start(const char* pConnAddr, unsigned short usConnPort, const char*pQueueAddr, unsigned short usQueuePort, const char* pHttpAddr, unsigned short usHttpPort);
	void	DoTick();
	void	Stop();

private:

	OSMutex							m_MutexRoom;
	OSMutex							m_MutexSession;

};

#endif	// __MsgServerMeeting_MRT_MEETING_H__