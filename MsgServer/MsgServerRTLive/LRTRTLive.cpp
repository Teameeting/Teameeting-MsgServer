#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "LRTRTLive.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "LRTRTLiveManager.h"
#include "LRTConnManager.h"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static LRTRTLive*	g_pRTLive = NULL;

void LRTRTLive::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void LRTRTLive::Initialize(int evTbSize)
{
	// Initialize utility classes
    LI("Hello server...\n");
	bool inDontFork = false;
	OS::Initialize();//initialize
	OSThread::Initialize();// initialize
    IdleTask::Initialize();// initialize IdleTaskThread

	// Initialize socket classes
	Socket::Initialize(evTbSize);// initialize EventThread
	SocketUtils::Initialize(!inDontFork);
#if !MACOSXEVENTQUEUE
	//Initialize the select() implementation of the event queue
	::select_startevents();
#endif

	{
		SInt32 numShortTaskThreads = 0;
		SInt32 numBlockingThreads = 0;
		SInt32 numThreads = 0;
		SInt32 numProcessors = 0;
		numProcessors = OS::GetNumProcessors();
		//////LI("Processer core Num:%lu\n",numProcessors);
		// Event thread need 1 position;
		numShortTaskThreads = numProcessors*2 - 1;
		if(numShortTaskThreads == 0)
			numShortTaskThreads = 2;
		numBlockingThreads = 1;
		numThreads = numShortTaskThreads + numBlockingThreads;
		// ////LI("Add threads shortask=%lu blocking=%lu\n",numShortTaskThreads, numBlockingThreads);
		TaskThreadPool::SetNumShortTaskThreads(numShortTaskThreads);
		TaskThreadPool::SetNumBlockingTaskThreads(numBlockingThreads);
		TaskThreadPool::AddThreads(numThreads);//initialize TaskThread & Start
	}

	TimeoutTask::Initialize();// initialize Task

	Socket::StartThread();// start EventThread
	OSThread::Sleep(100);
	g_pRTLive = new LRTRTLive();
	g_inited = true;
}

void LRTRTLive::DeInitialize()
{
	delete g_pRTLive;
	g_pRTLive = NULL;
	g_inited = false;
	TaskThreadPool::RemoveThreads();

	//Now, make sure that the server can't do any work
	TimeoutTask::UnInitialize();

	SocketUtils::UnInitialize();
    sleep(1);
    IdleTask::UnInitialize();

#if !MACOSXEVENTQUEUE
	::select_stopevents();
#endif

	Socket::Uninitialize();// uninitialize EventThread
	OS::UnInitialize();
    LI("ByeBye server...\n");
}

LRTRTLive* LRTRTLive::Inst()
{
	Assert(g_pRTLive != NULL);
	return g_pRTLive;
}

LRTRTLive::LRTRTLive(void)
: m_pRTLiveListener(NULL)
{

}

LRTRTLive::~LRTRTLive(void)
{
    if (m_pRTLiveListener)
    {
        delete m_pRTLiveListener;
        m_pRTLiveListener = NULL;
    }
}

int	LRTRTLive::Start(const char*pRTLiveIp, unsigned short usRTLivePort, const char*pConnectorIp, unsigned short usConnectorPort, const char*pLogicalIp, unsigned short usLogicalPort, const char*pDispatcherIp, unsigned short usDispatcherPort)
{
	Assert(g_inited);
	Assert(pRTLiveIp != NULL && strlen(pRTLiveIp)>0);
    LRTRTLiveManager::Instance().InitManager();
    LRTConnManager::Instance().InitManager();

	if(usLogicalPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", pLogicalIp, usLogicalPort);
        printf("logical addr:%s\n", addr);
        LRTConnManager::Instance().GetLogicalAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectLogical())) {
            LE("Start to ConnectLogical failed\n");
            return -1;
        }
	}

    if(usConnectorPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", pConnectorIp, usConnectorPort);
        printf("connector addr:%s\n", addr);
        LRTConnManager::Instance().GetConnectorAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectConnector())) {
            LE("Start to ConnectConnector failed\n");
            return -1;
        }
	}

    if(usDispatcherPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", pDispatcherIp, usDispatcherPort);
        printf("dispatcher addr:%s\n", addr);
        LRTConnManager::Instance().GetDispatcherAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectDispatcher())) {
            LE("Start to ConnectDispatcher failed\n");
            return -1;
        }
	}

    if (usRTLivePort > 0) {
        m_pRTLiveListener = new LRTRTLiveListener();
        OS_Error err = m_pRTLiveListener->Initialize(INADDR_ANY, usRTLivePort);
        if (err!=OS_NoErr) {
            LE("CreateRTLiveListener error port:%d\n", usRTLivePort);
            delete m_pRTLiveListener;
            m_pRTLiveListener = NULL;
            return -1;
        }
        LI("Start RTLive service:(%d) ok...,socketFD:%d\n", usRTLivePort, m_pRTLiveListener->GetSocketFD());
        m_pRTLiveListener->RequestEvent(EV_RE);
    }
	return 0;
}

void LRTRTLive::DoTick()
{
#if 1
    LRTConnManager::Instance().RefreshConnection();
#endif
}

void LRTRTLive::Stop()
{
    LRTConnManager::Instance().UninManager();
    LRTRTLiveManager::Instance().SignalKill();
    LRTRTLiveManager::Instance().ClearAll();
    LRTRTLiveManager::Instance().UninManager();
}
