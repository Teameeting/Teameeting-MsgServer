#ifndef __MsgServerRTLive_LRTRTLIVE_H__
#define __MsgServerRTLive_LRTRTLIVE_H__
#include "TCPListenerSocket.h"
#include "LRTRTLiveListener.h"

#include <iostream>
#include <map>
#include <vector>

class LRTRTLive
{
protected:
	LRTRTLive(void);
public:

	virtual ~LRTRTLive(void);

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
	static LRTRTLive* Inst();

public:
	int		Start(const char*pSequenceIp, unsigned short usSequencePort, const char*pStorageIp, unsigned short usStoragePort, const char*pModuleIp, unsigned short usModulePort);
	void	DoTick();
	void	Stop();

private:

    LRTRTLiveListener    *m_pModuleListener;

};

#endif	// __MsgServerRTLive_LRTRTLIVE_H__
