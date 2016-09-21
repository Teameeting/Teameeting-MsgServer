#include <iostream>
#include "MRTMeeting.h"
#include <stdio.h>
#include <assert.h>
#include "atomic.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "MRTConnManager.h"
#include "MRTRoomManager.h"
#include "RTUtils.hpp"

static bool         g_inited = false;
static const char*	g_pVersion = "0.01.20150810";
static MRTMeeting*	g_pMeeting = NULL;

void MRTMeeting::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void MRTMeeting::Initialize(int evTbSize)
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
	g_pMeeting = new MRTMeeting();
	g_inited = true;
}

void MRTMeeting::DeInitialize()
{
	delete g_pMeeting;
	g_pMeeting = NULL;
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

MRTMeeting* MRTMeeting::Inst()
{
	Assert(g_pMeeting != NULL);
	return g_pMeeting;
}

MRTMeeting::MRTMeeting(void)
{

}

MRTMeeting::~MRTMeeting(void)
{

}

int	MRTMeeting::Start(const MsConfigParser& conf)
{
	Assert(g_inited);
    int debug = conf.GetIntVal("global", "debug", 1);

    std::string strLocalIp("");
    std::string strHttpIp("");
    if (debug==1)
    {
        strLocalIp = conf.GetValue("global", "meeting_int_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_int_ip", "127.0.0.1");
    } else {
        strLocalIp = conf.GetValue("global", "meeting_ext_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_ext_ip", "127.0.0.1");
    }
    if (strLocalIp.length()==0 || strHttpIp.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int nConnPort = conf.GetIntVal("global", "listen_conn_port", 6610);
    int nDispPort = conf.GetIntVal("global", "listen_disp_port", 6620);
    int nHttpPort = conf.GetIntVal("resetful", "listen_http_port", 8055);

    int log_level = conf.GetIntVal("log", "level", 5);
    std::string strLogPath = conf.GetValue("log", "path");
    if (log_level < 0 || log_level > 5)
    {
        std::cout << "Error: Log level=" << log_level << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    char hh[24] = {0};
    sprintf(hh, "%s:%d", strHttpIp.c_str(), nHttpPort);

    std::string mid;
    GenericSessionId(mid);
    MRTConnManager::Instance().SetMeetingId(mid);
    LI("[][]MeetingId:%s\n", mid.c_str());

    char addr[24] = {0};
    sprintf(addr, "%s %u", strLocalIp.c_str(), nConnPort);
    MRTConnManager::Instance().GetAddrsList()->push_front(addr);

    if (!(MRTConnManager::Instance().ConnectConnector())) {
        LE("Start to ConnectConnector failed\n");
        return -1;
    }

    if (!(MRTRoomManager::Instance().Init(strLocalIp.c_str(), nDispPort, strHttpIp.c_str(), nHttpPort, hh))) {
        LE("Start to RoomManager Init failed\n");
        return -1;
    }

	return 0;
}

void MRTMeeting::DoTick()
{
    MRTConnManager::Instance().RefreshConnection();
    MRTRoomManager::Instance().CheckMembers();
    MRTRoomManager::Instance().SyncHttpRequest();
}

void MRTMeeting::Stop()
{
    MRTConnManager::Instance().SignalKill();
    MRTConnManager::Instance().ClearAll();
    MRTRoomManager::Instance().ClearAll();
}

