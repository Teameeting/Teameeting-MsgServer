#ifndef __RT_TCP_NO_TIMEOUT_H__
#define __RT_TCP_NO_TIMEOUT_H__
//INCLUDES
#include <unordered_map>
#include <utility>
#include <deque>
#include "LinkedList.h"
#include "StrPtrLen.h"
#include "TCPSocket.h"
#include "Task.h"
#include "TimeoutTask.h"
#include "RTObserverConnection.h"
#include "OSMutex.h"

enum
{
    kFirstTCPNoTimeoutSessionID     = 1,    //UInt32
};

class RTTcpNoTimeout
	: public Task
{
public:
	RTTcpNoTimeout(void);
	virtual ~RTTcpNoTimeout(void);

	TCPSocket*	GetSocket()	{ return &fSocket; }
	//Is this session alive? If this returns false, clean up and begone as
	//fast as possible
	Bool16 IsLiveSession()      { return fSocket.IsConnected(); }

	int SendData(const char*pData, int nLen);
	int SendTransferData(const char*pData, int nLen);
	void SetTimer(int time){Assert(time > 0);fTimeoutTask.SetTimeout(time);};
	void SetTickTimer(int time){Assert(time > 0);fTickTime = time;};
	void UpdateTimer(){fTimeoutTask.RefreshTimeout();};

    virtual void OnRecvData(const char*pData, int nLen) = 0;
    virtual void OnSendEvent(const char*pData, int nLen) = 0;
    virtual void OnWakeupEvent(const char*pData, int nLen) = 0;
    virtual void OnPushEvent(const char*pData, int nLen) = 0;
    virtual void OnTickEvent(const char*pData, int nLen) = 0;

protected:
	//* For Task
	virtual SInt64 Run();

    // Observer
    void AddObserver(RTObserverConnection* conn);
    void DelObserver(RTObserverConnection* conn);

	//CONSTANTS:
    enum
    {
        kRequestBufferSizeInBytes = 2048        //UInt32
    };
	//Each TCP session has a unique number that identifies it.
    UInt32              fSessionID;

private:
	TimeoutTask         fTimeoutTask;//allows the session to be timed out
	TCPSocket           fSocket;
	UInt32				fTickTime;

    std::deque<std::string>        m_dequeSend;
	List				m_listSend;
    OSMutex             mMutexSend;

    typedef std::unordered_map<RTTcpNoTimeout*, RTObserverConnection*> ObserverConnectionMap;
    typedef ObserverConnectionMap::iterator ObserverConnectionMapIt;
    ObserverConnectionMap m_mapConnectObserver;
    std::pair<ObserverConnectionMapIt, bool> m_OCMItPair;
	static unsigned int	 sSessionIDCounterNo;
};

#endif	// __RT_TCP_NO_TIMMEOUT_H__
