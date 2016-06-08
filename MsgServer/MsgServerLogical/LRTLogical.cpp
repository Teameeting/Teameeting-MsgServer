#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "LRTLogical.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "LRTLogicalManager.h"
#include "LRTConnManager.h"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static LRTLogical*	g_pLogical = NULL;

void LRTLogical::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void LRTLogical::Initialize(int evTbSize)
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
	g_pLogical = new LRTLogical();
	g_inited = true;
}

void LRTLogical::DeInitialize()
{
	delete g_pLogical;
	g_pLogical = NULL;
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

LRTLogical* LRTLogical::Inst()
{
	Assert(g_pLogical != NULL);
	return g_pLogical;
}

LRTLogical::LRTLogical(void)
: m_pLogicalListener(NULL)
{

}

LRTLogical::~LRTLogical(void)
{
    if (m_pLogicalListener)
    {
        delete m_pLogicalListener;
        m_pLogicalListener = NULL;
    }
}

int	LRTLogical::Start(const char*pSequenceIp, unsigned short usSequencePort, const char*pStorageIp, unsigned short usStoragePort, const char*pLogicalIp, unsigned short usLogicalPort)
{
	Assert(g_inited);
	Assert(pLogicalIp != NULL && strlen(pLogicalIp)>0);
    LRTLogicalManager::Instance().InitManager();

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
    printf("SequenceServer should already be connected!!!\n\n");

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
    printf("StorageServer should already be connected!!!\n\n");

    std::string ssid;
    if (usLogicalPort > 0) {
        m_pLogicalListener = new LRTLogicalListener();
        OS_Error err = m_pLogicalListener->Initialize(INADDR_ANY, usLogicalPort);
        if (err!=OS_NoErr) {
            LE("CreateLogicalListener error port:%d\n", usLogicalPort);
            delete m_pLogicalListener;
            m_pLogicalListener = NULL;
            return -1;
        }
        LI("Start Logical service:(%d) ok...,socketFD:%d\n", usLogicalPort, m_pLogicalListener->GetSocketFD());
        m_pLogicalListener->RequestEvent(EV_RE);
    }
	return 0;
}

void LRTLogical::DoTick()
{
#if 1
    //LRTLogicalManager::Instance().GenerateLogical();
#endif
}

void LRTLogical::Stop()
{
    LRTLogicalManager::Instance().SignalKill();
    LRTLogicalManager::Instance().ClearAll();
    LRTLogicalManager::Instance().UninManager();
}
