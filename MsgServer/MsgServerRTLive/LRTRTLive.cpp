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
static LRTRTLive*	g_pModule = NULL;

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
	g_pModule = new LRTRTLive();
	g_inited = true;
}

void LRTRTLive::DeInitialize()
{
	delete g_pModule;
	g_pModule = NULL;
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
	Assert(g_pModule != NULL);
	return g_pModule;
}

LRTRTLive::LRTRTLive(void)
: m_pModuleListener(NULL)
{

}

LRTRTLive::~LRTRTLive(void)
{
    if (m_pModuleListener)
    {
        delete m_pModuleListener;
        m_pModuleListener = NULL;
    }
}

int	LRTRTLive::Start(const char*pSequenceIp, unsigned short usSequencePort, const char*pStorageIp, unsigned short usStoragePort, const char*pModuleIp, unsigned short usModulePort)
{
	Assert(g_inited);
	Assert(pModuleIp != NULL && strlen(pModuleIp)>0);
    LRTRTLiveManager::Instance().InitManager();

    char *ip1 = "192.168.7.213";
    char *ip2 = "192.168.7.225";
    int port = 6379;

    char addr[24] = {0};
    sprintf(addr, "%s %d", ip1, port);
    LRTRTLiveManager::Instance().PushRedisHosts(addr);
    //memset(addr, 0, 24);
    //sprintf(addr, "%s %d", ip2, port);
    //LRTRTLiveManager::Instance().PushRedisHosts(addr);

	if(usSequencePort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", pSequenceIp, usSequencePort);
        LRTConnManager::Instance().GetSequenceAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectSequence())) {
            LE("Start to ConnectSequence failed\n");
            return -1;
        }
	}

    if(usStoragePort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", pStorageIp, usStoragePort);
        LRTConnManager::Instance().GetStorageAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectStorage())) {
            LE("Start to ConnectStorage failed\n");
            return -1;
        }
	}

    std::string ssid;
    if (usModulePort > 0) {
        m_pModuleListener = new LRTRTLiveListener();
        OS_Error err = m_pModuleListener->Initialize(INADDR_ANY, usModulePort);
        if (err!=OS_NoErr) {
            LE("CreateModuleListener error port:%d\n", usModulePort);
            delete m_pModuleListener;
            m_pModuleListener = NULL;
            return -1;
        }
        LI("Start Module service:(%d) ok...,socketFD:%d\n", usModulePort, m_pModuleListener->GetSocketFD());
        m_pModuleListener->RequestEvent(EV_RE);
    }
	return 0;
}

void LRTRTLive::DoTick()
{
#if 1
    //LRTRTLiveManager::Instance().GenerateModule();
#endif
}

void LRTRTLive::Stop()
{
    LRTRTLiveManager::Instance().SignalKill();
    LRTRTLiveManager::Instance().ClearAll();
    LRTRTLiveManager::Instance().UninManager();
}
