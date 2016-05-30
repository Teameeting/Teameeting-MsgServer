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
	
	static void PrintVersion();
	
	static void Initialize(int evTbSize);
	
	static void DeInitialize();

	static MRTMeeting* Inst();

public:
	int		Start(const char* pConnIp, unsigned short usConnPort, const char*pDispIp, unsigned short usDispPort, const char* pHttpIp, unsigned short usHttpPort);
	void	DoTick();
	void	Stop();

private:

};

#endif	// __MsgServerMeeting_MRT_MEETING_H__
