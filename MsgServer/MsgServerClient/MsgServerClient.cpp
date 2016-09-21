#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "MsgServerClient.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "ClientManager.h"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static MsgServerClient*	g_pSClient = NULL;

void MsgServerClient::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void MsgServerClient::Initialize(int evTbSize)
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
	g_pSClient = new MsgServerClient();
	g_inited = true;
}

void MsgServerClient::DeInitialize()
{
	delete g_pSClient;
	g_pSClient = NULL;
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

MsgServerClient* MsgServerClient::Inst()
{
	Assert(g_pSClient != NULL);
	return g_pSClient;
}

MsgServerClient::MsgServerClient(void)
{

}

MsgServerClient::~MsgServerClient(void)
{

}

int	MsgServerClient::Start(const char* userid, const char* ip, unsigned short port)
{
	Assert(g_inited);
    const char* groupid = "TestGroupId01";
    ClientManager::Instance().InitClient(groupid, userid, ip, port);
    // after connect server ok, do AddGroup in Callback
	return 0;
}

void MsgServerClient::DoTick()
{
#if 1
    ClientManager::Instance().SendGroupMsg();
#endif
}

void MsgServerClient::Stop()
{
}
