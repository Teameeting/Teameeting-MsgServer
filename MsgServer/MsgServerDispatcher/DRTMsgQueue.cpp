#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "DRTMsgQueue.h"
#include "atomic.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "DRTConnManager.h"
#include "RTUtils.hpp"


static bool         g_inited = false;
static const char*	g_pVersion = "0.01.20150810";
static DRTMsgQueue*	g_pMsgQueue = NULL;

void DRTMsgQueue::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void DRTMsgQueue::Initialize(int evTbSize)
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
	g_pMsgQueue = new DRTMsgQueue();
	g_inited = true;
}

void DRTMsgQueue::DeInitialize()
{
	delete g_pMsgQueue;
	g_pMsgQueue = NULL;
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

DRTMsgQueue* DRTMsgQueue::Inst()
{
	Assert(g_pMsgQueue != NULL);
	return g_pMsgQueue;
}

DRTMsgQueue::DRTMsgQueue(void)
: m_pModuleListener(NULL)
, m_pTransferSession(NULL)
{

}

DRTMsgQueue::~DRTMsgQueue(void)
{
    if (m_pTransferSession) {
        delete m_pTransferSession;
        m_pTransferSession = NULL;
    }

    if (m_pModuleListener) {
        delete m_pModuleListener;
        m_pModuleListener = NULL;
    }
}

int	DRTMsgQueue::Start(const MsConfigParser& conf)
{
	Assert(g_inited);
    int debug = conf.GetIntVal("global", "debug", 1);

    std::string strLocalIp("");
    std::string strHttpIp("");
    if (debug==1)
    {
        strLocalIp = conf.GetValue("global", "dispatcher_int_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_int_ip", "127.0.0.1");
    } else {
        strLocalIp = conf.GetValue("global", "dispatcher_ext_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_ext_ip", "127.0.0.1");
    }
    if (strLocalIp.length()==0 || strHttpIp.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int nConnPort = conf.GetIntVal("global", "listen_conn_port", 6620);
    int nDispPort = conf.GetIntVal("global", "listen_disp_port", 6640);
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

    DRTConnManager::s_cohttpHost = hh;
    DRTConnManager::s_cohttpIp = strHttpIp.c_str();
    DRTConnManager::s_cohttpPort = nHttpPort;

    std::string mid;
    GenericSessionId(mid);
    DRTConnManager::Instance().SetMsgQueueId(mid);
    LI("[][]MsgQueueId:%s\n", mid.c_str());

	if(nConnPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", strLocalIp.c_str(), nConnPort);
        DRTConnManager::Instance().GetAddrsList()->push_front(addr);

        if (!(DRTConnManager::Instance().ConnectConnector())) {
            LE("Start to ConnectConnector failed\n");
            return -1;
        }
	}

	if(nDispPort > 0)
	{
        m_pModuleListener = new DRTModuleListener();
        OS_Error err = m_pModuleListener->Initialize(INADDR_ANY, nDispPort);
        if (err!=OS_NoErr)
        {
            LE("CreateMeetingListener error port=%d \n", nDispPort);
            delete m_pModuleListener;
            m_pModuleListener=NULL;
            return -1;
        }
        LI("Start MsgQueue service meet:(%d) ok...,socketFD:%d\n", nDispPort, m_pModuleListener->GetSocketFD());
        m_pModuleListener->RequestEvent(EV_RE);
	}

   return 0;
}

void DRTMsgQueue::DoTick()
{
#if 1
    DRTConnManager::Instance().RefreshConnection();
#endif
}

void DRTMsgQueue::Stop()
{
    DRTConnManager::Instance().SignalKill();
    DRTConnManager::Instance().ClearAll();
}

