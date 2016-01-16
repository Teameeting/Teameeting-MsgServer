#ifndef __MsgServerDispatcher_DRTMEETING_H__
#define __MsgServerDispatcher_DRTMEETING_H__
#include "TCPListenerSocket.h"
#include "DRTModuleListener.h"
#include "DRTTransferSession.h"
#include <iostream>

class DRTMsgQueue
{
protected:
	DRTMsgQueue(void);
public:
	virtual ~DRTMsgQueue(void);

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
	static DRTMsgQueue* Inst();
    

public:
	int		Start(const char*pConnIp, unsigned short usConnPort, const char*pModuleIp, unsigned short usModulePort);
	void	DoTick();
	void	Stop();

private:
    DRTModuleListener                *m_pModuleListener;
    DRTTransferSession               *m_pTransferSession;

	OSMutex							m_MutexRoom;
	OSMutex							m_MutexSession;

};

#endif	// __MsgServerDispatcher_DRTMEETING_H__