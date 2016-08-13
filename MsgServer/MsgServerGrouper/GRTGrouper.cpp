#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "GRTGrouper.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "GRTGrouperManager.h"
#include "GRTConnManager.h"
#include "GRTChannelManager.h"
#include "GRTSubChannel.h"

#include "GRTTransferSession.h"

static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static GRTGrouper*	g_pGrouper = NULL;

void GRTGrouper::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void GRTGrouper::Initialize(int evTbSize)
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
	g_pGrouper = new GRTGrouper();
	g_inited = true;
}

void GRTGrouper::DeInitialize()
{
	delete g_pGrouper;
	g_pGrouper = NULL;
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

GRTGrouper* GRTGrouper::Inst()
{
	Assert(g_pGrouper != NULL);
	return g_pGrouper;
}

GRTGrouper::GRTGrouper(void)
: m_pGrouperListener(NULL)
{

}

GRTGrouper::~GRTGrouper(void)
{
    if (m_pGrouperListener)
    {
        delete m_pGrouperListener;
        m_pGrouperListener = NULL;
    }
}

int	GRTGrouper::Start(const MsConfigParser& conf)
{
	Assert(g_inited);
    int debug = conf.GetIntVal("global", "debug", 1);

    std::string strLocalIp("");
    std::string strHttpIp("");
    std::string strRedisIp1("");
    if (debug==1)
    {
        strLocalIp = conf.GetValue("global", "grouper_int_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_int_ip", "127.0.0.1");
        strRedisIp1 = conf.GetValue("redis", "redis_int_ip1", "127.0.0.1");
    } else {
        strLocalIp = conf.GetValue("global", "grouper_ext_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_ext_ip", "127.0.0.1");
        strRedisIp1 = conf.GetValue("redis", "redis_ext_ip1", "127.0.0.1");
    }
    if (strLocalIp.length()==0 || strHttpIp.length()==0 || strRedisIp1.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int nGrouperPort = conf.GetIntVal("global", "listen_grouper_port", 6692);
    int nGroupmgrPort = conf.GetIntVal("global", "listen_groupmgr_port", 6690);
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

    GRTGrouperManager::Instance().InitManager(strRedisIp1, nRedisPort1);
    GRTChannelManager::Instance().InitManager();

    std::string channel1("follow_group");
    std::string channel2("unfollow_group");
    std::string chKey1;
    std::string chKey2;
    GRTSubChannel* ch1 = GRTChannelManager::Instance().GenSubChannel(strRedisIp1, nRedisPort1, channel1, chKey1);
    GRTSubChannel* ch2 = GRTChannelManager::Instance().GenSubChannel(strRedisIp1, nRedisPort1, channel2, chKey2);

    if (ch1) {
        GRTChannelManager::Instance().AddChannel(chKey1, ch1);
    } else {
        LE("GRTChannelManager GenSubChannel ch1 return null\n");
        Assert(false);
    }
    if (ch2) {
        GRTChannelManager::Instance().AddChannel(chKey2, ch2);
    } else {
        LE("GRTChannelManager GenSubChannel ch2 return null\n");
        Assert(false);
    }

    std::string ssid;
    if (nGrouperPort > 0) {
        m_pGrouperListener = new GRTGrouperListener();
        OS_Error err = m_pGrouperListener->Initialize(INADDR_ANY, nGrouperPort);
        if (err!=OS_NoErr) {
            LE("CreateGrouperListener error port:%d\n", nGrouperPort);
            delete m_pGrouperListener;
            m_pGrouperListener = NULL;
            return -1;
        }
        LI("Start Grouper service:(%d) ok...,socketFD:%d\n", nGrouperPort, m_pGrouperListener->GetSocketFD());
        m_pGrouperListener->RequestEvent(EV_RE);
    }

    if(nGroupmgrPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", strLocalIp.c_str(), nGroupmgrPort);
        GRTConnManager::Instance().GetGroupMgrAddrList()->push_front(addr);

        LI("ConnectGroupMgr was called...\n");
        if (!(GRTConnManager::Instance().ConnectGroupMgr())) {
            LE("Start to ConnectGroupMgr failed\n");
            return -1;
        }
	}

    LI("Start Grouper Service success...\n");
	return 0;
}

void GRTGrouper::DoTick()
{
#if 0

#endif
}

void GRTGrouper::Stop()
{
    GRTGrouperManager::Instance().SignalKill();
    GRTGrouperManager::Instance().ClearAll();
    GRTGrouperManager::Instance().UninManager();
    GRTChannelManager::Instance().UninManager();
}
