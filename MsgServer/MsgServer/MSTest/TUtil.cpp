#include <iostream>
#include "TUtil.h"
#include <stdio.h>
#include <assert.h>
#include "atomic.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "TTask.h"

static bool         g_inited = false;
static TUtil*	    g_pTutil = NULL;

void TUtil::Initialize(int evTbSize)
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
        printf("====>AddThreads num:%d\n", numThreads);
		TaskThreadPool::AddThreads(numThreads);//initialize TaskThread & Start
	}

	TimeoutTask::Initialize();// initialize Task

	Socket::StartThread();// start EventThread
	OSThread::Sleep(100);
	g_pTutil = new TUtil();
	g_inited = true;
}

void TUtil::DeInitialize()
{
	delete g_pTutil;
	g_pTutil = NULL;
	g_inited = false;
    std::cout << "TUtil::DeInitialize call TaskThreadPool::RemoveThreads" << std::endl;
	TaskThreadPool::RemoveThreads();

	//Now, make sure that the server can't do any work
    std::cout << "TUtil::DeInitialize call TimeoutTask::UnInitialize" << std::endl;
	TimeoutTask::UnInitialize();

    std::cout << "TUtil::DeInitialize call SocketUtils::UnInitialize" << std::endl;
	SocketUtils::UnInitialize();
    IdleTask::UnInitialize();

#if !MACOSXEVENTQUEUE
	::select_stopevents();
#endif

    std::cout << "TUtil::DeInitialize call Socket::UnInitialize" << std::endl;
	Socket::Uninitialize();// uninitialize EventThread
    std::cout << "TUtil::DeInitialize call OS::UnInitialize" << std::endl;
	OS::UnInitialize();
    LI("ByeBye server...");
}

TUtil* TUtil::Inst()
{
	Assert(g_pTutil != NULL);
	return g_pTutil;
}

TUtil::TUtil(void)
:tt1(NULL)
,tt2(NULL)
,tt3(NULL){

}

TUtil::~TUtil(void)
{

}

int	TUtil::Start()
{
    tt1 = new TTask();
    tt2 = new TTask();
    tt3 = new TTask();
	return 0;
}

void TUtil::DoTick()
{

}

void TUtil::Stop()
{
    //tt1->Signal(Task::kKillEvent);
    //tt2->Signal(Task::kKillEvent);
    //tt3->Signal(Task::kKillEvent);
}

