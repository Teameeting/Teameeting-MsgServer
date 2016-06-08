#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "SRTStorage.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "SRTStorageManager.h"
#include "SRTStorageRedis.h"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static SRTStorage*	g_pStorage = NULL;

void SRTStorage::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void SRTStorage::Initialize(int evTbSize)
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
	g_pStorage = new SRTStorage();
	g_inited = true;
}

void SRTStorage::DeInitialize()
{
	delete g_pStorage;
	g_pStorage = NULL;
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

SRTStorage* SRTStorage::Inst()
{
	Assert(g_pStorage != NULL);
	return g_pStorage;
}

SRTStorage::SRTStorage(void)
: m_pStorageListener(NULL)
{

}

SRTStorage::~SRTStorage(void)
{
    if (m_pStorageListener)
    {
        delete m_pStorageListener;
        m_pStorageListener = NULL;
    }
}

int	SRTStorage::Start(const char*pStorageIp, unsigned short usStoragePort)
{
	Assert(g_inited);
	Assert(pStorageIp != NULL && strlen(pStorageIp)>0);
    SRTStorageManager::Instance().InitManager();

    char *ip1 = "192.168.7.213";
    char *ip2 = "192.168.7.225";
    int port = 6379;

    char addr[24] = {0};
    sprintf(addr, "%s %d", ip1, port);
    SRTStorageManager::Instance().PushRedisHosts(addr);

    std::string ssid;
    if (usStoragePort > 0) {
        m_pStorageListener = new SRTStorageListener();
        OS_Error err = m_pStorageListener->Initialize(INADDR_ANY, usStoragePort);
        if (err!=OS_NoErr) {
            LE("CreateStorageListener error port:%d\n", usStoragePort);
            delete m_pStorageListener;
            m_pStorageListener = NULL;
            return -1;
        }
        LI("Start Storage service:(%d) ok...,socketFD:%d\n", usStoragePort, m_pStorageListener->GetSocketFD());
        m_pStorageListener->RequestEvent(EV_RE);
    }
	return 0;
}

void SRTStorage::DoTick()
{
#if 1
    //SRTStorageManager::Instance().GenerateStorage();
#endif
}

void SRTStorage::Stop()
{
    SRTStorageManager::Instance().SignalKill();
    SRTStorageManager::Instance().ClearAll();
    SRTStorageManager::Instance().UninManager();
}
