#ifndef __MsgServerStorage_SRTSTORAGE_H__
#define __MsgServerStorage_SRTSTORAGE_H__
#include "TCPListenerSocket.h"
#include "SRTStorageListener.h"
#include "MsConfigParser.h"

#include <iostream>
#include <map>
#include <vector>

class SRTStorage
{
protected:
	SRTStorage(void);
public:

	virtual ~SRTStorage(void);

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
	static SRTStorage* Inst();

public:
	int		Start(const MsConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    SRTStorageListener    *m_pStorageListener;

};

#endif	// __MsgServerStorage_SRTSTORAGE_H__
