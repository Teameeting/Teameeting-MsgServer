#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "SRTSequence.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "SRTSequenceManager.h"
#include "RTHiredis.h"
#include "SRTSequenceRedis.h"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static SRTSequence*	g_pSequence = NULL;

void SRTSequence::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void SRTSequence::Initialize(int evTbSize)
{
	// Initialize utility classes
    //////LI("Hello server...\n");
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
	g_pSequence = new SRTSequence();
	g_inited = true;
}

void SRTSequence::DeInitialize()
{
	delete g_pSequence;
	g_pSequence = NULL;
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
    //////LI("ByeBye server...\n");
}

SRTSequence* SRTSequence::Inst()
{
	Assert(g_pSequence != NULL);
	return g_pSequence;
}

SRTSequence::SRTSequence(void)
: m_pSequenceListener(NULL)
{

}

SRTSequence::~SRTSequence(void)
{
    if (m_pSequenceListener)
    {
        delete m_pSequenceListener;
        m_pSequenceListener = NULL;
    }
}

int	SRTSequence::Start(const char*pSequenceIp, unsigned short usSequencePort)
{
	Assert(g_inited);
	Assert(pSequenceIp != NULL && strlen(pSequenceIp)>0);
    SRTSequenceManager::Instance().InitManager();

    char *ip1 = "192.168.7.213";
    char *ip2 = "192.168.7.225";
    int port = 6379;

    char addr[24] = {0};
    sprintf(addr, "%s %d", ip1, port);
    SRTSequenceManager::Instance().PushRedisHosts(addr);
    memset(addr, 0, 24);
    sprintf(addr, "%s %d", ip2, port);
    SRTSequenceManager::Instance().PushRedisHosts(addr);

    std::string ssid;
    LI("SRTSequence ip:%s, port:%u\n", pSequenceIp, usSequencePort);
    if (usSequencePort > 0) {
        m_pSequenceListener = new SRTSequenceListener();
        OS_Error err = m_pSequenceListener->Initialize(INADDR_ANY, usSequencePort);
        if (err!=OS_NoErr) {
            LE("CreateSequenceListener error port:%d\n", usSequencePort);
            delete m_pSequenceListener;
            m_pSequenceListener = NULL;
            return -1;
        }
        LI("Start Sequence service:(%d) ok...,socketFD:%d\n", usSequencePort, m_pSequenceListener->GetSocketFD());
        m_pSequenceListener->RequestEvent(EV_RE);
    }
	return 0;
}

void SRTSequence::DoTick()
{
#if 1
    //SRTSequenceManager::Instance().GenerateSequence();
#endif
}

void SRTSequence::Stop()
{
    SRTSequenceManager::Instance().SignalKill();
    SRTSequenceManager::Instance().ClearAll();
    SRTSequenceManager::Instance().UninManager();
    //SRTSequenceManager::Instance().ClearRedisServer();
    //RTHiredisRemote::Instance().DisConn();
}
