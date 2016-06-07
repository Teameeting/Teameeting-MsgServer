#ifndef __MsgServerModule_MRTLOGICAL_H__
#define __MsgServerModule_MRTLOGICAL_H__
#include "TCPListenerSocket.h"
#include "MRTModuleListener.h"

#include <iostream>
#include <map>
#include <vector>

class MRTModule
{
protected:
	MRTModule(void);
public:

	virtual ~MRTModule(void);

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
	static MRTModule* Inst();

public:
	int		Start(const char*pSequenceIp, unsigned short usSequencePort, const char*pStorageIp, unsigned short usStoragePort, const char*pModuleIp, unsigned short usModulePort);
	void	DoTick();
	void	Stop();

private:

    MRTModuleListener    *m_pModuleListener;

};

#endif	// __MsgServerModule_MRTLOGICAL_H__
