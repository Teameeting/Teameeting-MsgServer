#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "PRTPusher.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "PRTPusherManager.h"
#include "PRTConnManager.h"
#include "IosPusher.h"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static PRTPusher*	g_pModule = NULL;

void PRTPusher::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void PRTPusher::Initialize(int evTbSize)
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
	g_pModule = new PRTPusher();
	g_inited = true;
}

void PRTPusher::DeInitialize()
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

PRTPusher* PRTPusher::Inst()
{
	Assert(g_pModule != NULL);
	return g_pModule;
}

PRTPusher::PRTPusher(void)
: m_pModuleListener(NULL)
{

}

PRTPusher::~PRTPusher(void)
{
    if (m_pModuleListener)
    {
        delete m_pModuleListener;
        m_pModuleListener = NULL;
    }
}

int	PRTPusher::Start(const MsConfigParser& conf)
{
	Assert(g_inited);
    int debug = conf.GetIntVal("global", "debug", 1);

    std::string strLocalIp("");
    std::string strHttpIp("");
    std::string strRedisIp1("");
    if (debug)
    {
        strLocalIp = conf.GetValue("global", "pusher_int_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_int_ip", "127.0.0.1");
        strRedisIp1 = conf.GetValue("redis", "redis_int_ip1", "127.0.0.1");
    } else {
        strLocalIp = conf.GetValue("global", "pusher_ext_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_ext_ip", "127.0.0.1");
        strRedisIp1 = conf.GetValue("redis", "redis_ext_ip1", "127.0.0.1");
    }
    if (strLocalIp.length()==0 || strRedisIp1.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int nConnectorPort = conf.GetIntVal("global", "listen_connector_port", 6620);
    int nRtlivePusherPort = conf.GetIntVal("global", "listen_rtlivepusher_port", 6694);
    int nHttpPort = conf.GetIntVal("resetful", "listen_http_port", 9999);
    int nRedisPort1 = conf.GetIntVal("redis", "redis_port1", 6379);

    int log_level = conf.GetIntVal("log", "level", 5);
    std::string strLogPath = conf.GetValue("log", "path");
    if (log_level < 0 || log_level > 5)
    {
        std::cout << "Error: Log level=" << log_level << " extend range(0 - 5)!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    PRTPusherManager::Instance().InitManager();

    char addr[24] = {0};
    sprintf(addr, "%s %d", strRedisIp1.c_str(), nRedisPort1);
    PRTPusherManager::Instance().PushRedisHosts(addr);

	if(nConnectorPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", strLocalIp.c_str(), nConnectorPort);
        PRTConnManager::Instance().GetConnectorAddrList()->push_front(addr);

        if (!(PRTConnManager::Instance().ConnectConnector())) {
            LE("Start to ConnectConnector failed\n");
            return -1;
        }
	}

    if(nRtlivePusherPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", strLocalIp.c_str(), nRtlivePusherPort);
        PRTConnManager::Instance().GetRtlivePusherAddrList()->push_front(addr);

        if (!(PRTConnManager::Instance().ConnectRtlivePusher())) {
            LE("Start to ConnectRtlivePusher failed\n");
            return -1;
        }
	}

	return 0;
}

void PRTPusher::DoTick()
{
#if 1
    PRTConnManager::Instance().RefreshConnection();
#endif
}

void PRTPusher::Stop()
{
    PRTPusherManager::Instance().SignalKill();
    PRTPusherManager::Instance().ClearAll();
    PRTPusherManager::Instance().UninManager();
    PRTConnManager::Instance().SignalKill();
    PRTConnManager::Instance().ClearAll();
}
