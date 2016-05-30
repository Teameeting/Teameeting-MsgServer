#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "RRTRTRedis.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "RRTRTRedisManager.h"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static RRTRTRedis*	g_pRTRedis = NULL;

void RRTRTRedis::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void RRTRTRedis::Initialize(int evTbSize)
{
	// Initialize utility classes
    LI("Hello server...");
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
		LI("Processer core Num:%lu\n",numProcessors);
		// Event thread need 1 position;
		numShortTaskThreads = numProcessors*2 - 1;
		if(numShortTaskThreads == 0)
			numShortTaskThreads = 2;
		numBlockingThreads = 1;
		numThreads = numShortTaskThreads + numBlockingThreads;
		// LI("Add threads shortask=%lu blocking=%lu\n",numShortTaskThreads, numBlockingThreads);
		TaskThreadPool::SetNumShortTaskThreads(numShortTaskThreads);
		TaskThreadPool::SetNumBlockingTaskThreads(numBlockingThreads);
		TaskThreadPool::AddThreads(numThreads);//initialize TaskThread & Start
	}

	TimeoutTask::Initialize();// initialize Task

	Socket::StartThread();// start EventThread
	OSThread::Sleep(100);
	g_pRTRedis = new RRTRTRedis();
	g_inited = true;
}

void RRTRTRedis::DeInitialize()
{
	delete g_pRTRedis;
	g_pRTRedis = NULL;
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
    LI("ByeBye server...");
}

RRTRTRedis* RRTRTRedis::Inst()
{
	Assert(g_pRTRedis != NULL);
	return g_pRTRedis;
}

RRTRTRedis::RRTRTRedis(void)
: m_pRTRedisListener(NULL)
{

}

RRTRTRedis::~RRTRTRedis(void)
{
    if (m_pRTRedisListener)
    {
        delete m_pRTRedisListener;
        m_pRTRedisListener = NULL;
    }
}

int	RRTRTRedis::Start(const char*pRTRedisIp, unsigned short usRTRedisPort)
{
	Assert(g_inited);
	Assert(pRTRedisIp != NULL && strlen(pRTRedisIp)>0);

    std::string ssid;

    if (usRTRedisPort > 0) {
        m_pRTRedisListener = new RRTRTRedisListener();
        OS_Error err = m_pRTRedisListener->Initialize(INADDR_ANY, usRTRedisPort);
        if (err!=OS_NoErr) {
            LE("CreateRTRedisListener error port:%d\n", usRTRedisPort);
            delete m_pRTRedisListener;
            m_pRTRedisListener = NULL;
            return -1;
        }
        LI("Start RTRedis service:(%d) ok...,socketFD:%d\n", usRTRedisPort, m_pRTRedisListener->GetSocketFD());
        m_pRTRedisListener->RequestEvent(EV_RE);
    }
	return 0;
}

void RRTRTRedis::DoTick()
{
#if 0

#endif
}

void RRTRTRedis::Stop()
{
    RRTRTRedisManager::Instance().SignalKill();
    RRTRTRedisManager::Instance().ClearAll();
}
