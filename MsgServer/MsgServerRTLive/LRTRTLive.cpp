#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "LRTRTLive.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"
#include "LRTRTLiveManager.h"
#include "LRTConnManager.h"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static LRTRTLive*	g_pRTLive = NULL;

void LRTRTLive::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void LRTRTLive::Initialize(int evTbSize)
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
	g_pRTLive = new LRTRTLive();
	g_inited = true;
}

void LRTRTLive::DeInitialize()
{
	delete g_pRTLive;
	g_pRTLive = NULL;
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

LRTRTLive* LRTRTLive::Inst()
{
	Assert(g_pRTLive != NULL);
	return g_pRTLive;
}

LRTRTLive::LRTRTLive(void)
: m_pRTLiveListener(NULL)
{

}

LRTRTLive::~LRTRTLive(void)
{
    if (m_pRTLiveListener)
    {
        delete m_pRTLiveListener;
        m_pRTLiveListener = NULL;
    }
}

int	LRTRTLive::Start(const MsConfigParser& conf)
{
	Assert(g_inited);
    int debug = conf.GetIntVal("global", "debug", 1);

    std::string strLocalIp("");
    std::string strHttpIp("");
    if (debug==1)
    {
        strLocalIp = conf.GetValue("global", "rtlive_int_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_int_ip", "127.0.0.1");
    } else {
        strLocalIp = conf.GetValue("global", "rtlive_ext_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_ext_ip", "127.0.0.1");
    }
    if (strLocalIp.length()==0 || strHttpIp.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int nGrouperPort = conf.GetIntVal("global", "listen_grouper_port", 6690);
    int nRtlivePort = conf.GetIntVal("global", "listen_rtlive_port", 6680);
    int nConnectorPort = conf.GetIntVal("global", "listen_connector_port", 6620);
    int nLogicalPort = conf.GetIntVal("global", "listen_logical_port", 6670);
    int nDispatcherPort = conf.GetIntVal("global", "listen_dispatcher_port", 6640);
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
    LRTRTLiveManager::Instance().InitManager();
    LRTConnManager::Instance().InitManager();

	if(nLogicalPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", strLocalIp.c_str(), nLogicalPort);
        LI("logical addr:%s\n", addr);
        LRTConnManager::Instance().GetLogicalAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectLogical())) {
            LE("Start to ConnectLogical failed\n");
            return -1;
        }
	}

    if(nConnectorPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", strLocalIp.c_str(), nConnectorPort);
        LI("connector addr:%s\n", addr);
        LRTConnManager::Instance().GetConnectorAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectConnector())) {
            LE("Start to ConnectConnector failed\n");
            return -1;
        }
	}

    if(nDispatcherPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", strLocalIp.c_str(), nDispatcherPort);
        LI("dispatcher addr:%s\n", addr);
        LRTConnManager::Instance().GetDispatcherAddrList()->push_front(addr);

        if (!(LRTConnManager::Instance().ConnectDispatcher())) {
            LE("Start to ConnectDispatcher failed\n");
            return -1;
        }
	}

    if (nRtlivePort > 0) {
        m_pRTLiveListener = new LRTRTLiveListener();
        OS_Error err = m_pRTLiveListener->Initialize(INADDR_ANY, nRtlivePort);
        if (err!=OS_NoErr) {
            LE("CreateRTLiveListener error port:%d\n", nRtlivePort);
            delete m_pRTLiveListener;
            m_pRTLiveListener = NULL;
            return -1;
        }
        LI("Start RTLive service:(%d) ok...,socketFD:%d\n", nRtlivePort, m_pRTLiveListener->GetSocketFD());
        m_pRTLiveListener->RequestEvent(EV_RE);
    }

    if (nGrouperPort > 0) {
        m_pGroupListener = new LRTGroupListener();
        OS_Error err = m_pGroupListener->Initialize(INADDR_ANY, nGrouperPort);
        if (err!=OS_NoErr) {
            LE("CreateGroupListener error port:%d\n", nGrouperPort);
            delete m_pGroupListener;
            m_pGroupListener = NULL;
            return -1;
        }
        LI("Start RTLive group service:(%d) ok...,socketFD:%d\n", nGrouperPort, m_pGroupListener->GetSocketFD());
        m_pGroupListener->RequestEvent(EV_RE);
    }
	return 0;
}

void LRTRTLive::DoTick()
{
#if 1
    LRTConnManager::Instance().RefreshConnection();
#endif
}

void LRTRTLive::Stop()
{
    LRTConnManager::Instance().UninManager();
    LRTRTLiveManager::Instance().SignalKill();
    LRTRTLiveManager::Instance().ClearAll();
    LRTRTLiveManager::Instance().UninManager();
}
