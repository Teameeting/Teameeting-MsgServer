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

int	SRTStorage::Start(const MsConfigParser& conf)
{
	Assert(g_inited);
    int debug = conf.GetIntVal("global", "debug", 1);

    std::string strLocalIp("");
    std::string strHttpIp("");
    std::string strRedisIp1("");
    if (debug==1)
    {
        strLocalIp = conf.GetValue("global", "storage_int_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_int_ip", "127.0.0.1");
        strRedisIp1 = conf.GetValue("redis", "redis_int_ip1", "127.0.0.1");
    } else {
        strLocalIp = conf.GetValue("global", "storage_ext_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_ext_ip", "127.0.0.1");
        strRedisIp1 = conf.GetValue("redis", "redis_ext_ip1", "127.0.0.1");
    }
    if (strLocalIp.length()==0 || strHttpIp.length()==0 || strRedisIp1.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int nStoragePort = conf.GetIntVal("global", "listen_storage_port", 6660);
    int nHttpPort = conf.GetIntVal("resetful", "listen_http_port", 8055);
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

    SRTStorageManager::Instance().InitManager();

    char addr[24] = {0};
    sprintf(addr, "%s %d", strRedisIp1.c_str(), nRedisPort1);
    SRTStorageManager::Instance().PushRedisHosts(addr);

    std::string ssid;
    if (nStoragePort > 0) {
        m_pStorageListener = new SRTStorageListener();
        OS_Error err = m_pStorageListener->Initialize(INADDR_ANY, nStoragePort);
        if (err!=OS_NoErr) {
            LE("CreateStorageListener error port:%d\n", nStoragePort);
            delete m_pStorageListener;
            m_pStorageListener = NULL;
            return -1;
        }
        LI("Start Storage service:(%d) ok...,socketFD:%d\n", nStoragePort, m_pStorageListener->GetSocketFD());
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
