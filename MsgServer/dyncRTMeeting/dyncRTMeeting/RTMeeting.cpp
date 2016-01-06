#include "RTMeeting.h"
#include <stdio.h>
#include <assert.h>
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"
#include "RTConnectionManager.h"
#include "RTRoomManager.h"
#include <iostream>
#include "RTHiredis.h"

static bool         g_inited = false;
static const char*	g_pVersion = "0.01.20150810";
static RTMeeting*	g_pMeeting = NULL;

void RTMeeting::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void RTMeeting::Initialize(int evTbSize)
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
	g_pMeeting = new RTMeeting();
	g_inited = true;
}

void RTMeeting::DeInitialize()
{
	delete g_pMeeting;
	g_pMeeting = NULL;
	g_inited = false;
	TaskThreadPool::RemoveThreads();

	//Now, make sure that the server can't do any work
	TimeoutTask::UnInitialize();

	SocketUtils::UnInitialize();

#if !MACOSXEVENTQUEUE
	::select_stopevents();
#endif

	OS::UnInitialize();
    LI("ByeBye server...");
}

RTMeeting* RTMeeting::Inst()
{
	Assert(g_pMeeting != NULL);
	return g_pMeeting;
}

RTMeeting::RTMeeting(void)
{

}

RTMeeting::~RTMeeting(void)
{
    
}

int	RTMeeting::Start(const char*pConnAddr, unsigned short usConnPort, const char*pQueueAddr, unsigned short usQueuePort, const char* pHttpAddr, unsigned short usHttpPort)
{
	Assert(g_inited);
	Assert(pConnAddr != NULL && strlen(pConnAddr)>0);
	Assert(pQueueAddr != NULL && strlen(pQueueAddr)>0);
	Assert(pHttpAddr != NULL && strlen(pHttpAddr)>0);
    char hh[24] = {0};
    sprintf(hh, "%s:%u", pHttpAddr, usHttpPort);
    RTRoomManager::s_msgQueueIp = pQueueAddr;
    RTRoomManager::s_msgQueuePort = usQueuePort;
    RTRoomManager::s_httpHost = hh;
    RTRoomManager::s_httpIp = pHttpAddr;
    RTRoomManager::s_httpPort = usHttpPort;

    RTHiredisLocal::Instance()->Connect();
    std::string mid;
    RTConnectionManager::Instance()->GenericSessionId(mid);
    RTConnectionManager::Instance()->SetMeetingId(mid);
    LI("[][]MeetingId:%s\n", mid.c_str());

    char addr[24] = {0};
    sprintf(addr, "%s %u", pConnAddr, usConnPort);
    RTConnectionManager::Instance()->GetAddrsList()->push_front(addr);
    
    if (!(RTConnectionManager::Instance()->ConnectConnector())) {
        LE("Start to ConnectConnector failed\n");
        return -1;
    }
    
    if (!(RTRoomManager::Instance()->Init())) {
        LE("Start to RoomManager Init failed\n");
        return -1;
    }

	return 0;
}

void RTMeeting::DoTick()
{
    RTConnectionManager::Instance()->RefreshConnection();
    RTRoomManager::Instance()->RefreshConnection();
    RTRoomManager::Instance()->CheckMembers();
    RTRoomManager::Instance()->SyncHttpRequest();
}

void RTMeeting::Stop()
{

}

