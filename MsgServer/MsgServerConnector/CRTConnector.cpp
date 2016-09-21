#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "CRTConnector.h"
#include "CRTConnection.h"
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
#include "CRTConnManager.h"
#include "CRTDispatchConnection.h"
#include "RTUtils.hpp"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static CRTConnector*	g_pConnector = NULL;

void CRTConnector::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void CRTConnector::Initialize(int evTbSize)
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
	g_pConnector = new CRTConnector();
	g_inited = true;
}

void CRTConnector::DeInitialize()
{
	delete g_pConnector;
	g_pConnector = NULL;
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

CRTConnector* CRTConnector::Inst()
{
	Assert(g_pConnector != NULL);
	return g_pConnector;
}

CRTConnector::CRTConnector(void)
: m_pConnListener(NULL)
, m_pModuleListener(NULL)
, m_pConnTcpListener(NULL)
, m_pWebSvrListener(NULL)
{

}

CRTConnector::~CRTConnector(void)
{
    if (m_pWebSvrListener) {
        delete m_pWebSvrListener;
        m_pWebSvrListener = NULL;
    }
    if (m_pConnTcpListener) {
        delete m_pConnTcpListener;
        m_pConnTcpListener = NULL;
    }
    if (m_pModuleListener) {
        delete m_pModuleListener;
        m_pModuleListener = NULL;
    }
	if(m_pConnListener)
	{
		delete m_pConnListener;
		m_pConnListener = NULL;
	}
}

int	CRTConnector::Start(const MsConfigParser& conf)
{
	Assert(g_inited);

    int debug = conf.GetIntVal("global", "debug", 1);

    std::string strLocalIp("");
    std::string strHttpIp("");
    std::string strRedisIp1("");
    if (debug==1)
    {
        strLocalIp = conf.GetValue("global", "connector_int_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_int_ip", "127.0.0.1");
        strRedisIp1 = conf.GetValue("redis", "redis_int_ip1", "127.0.0.1");
    } else {
        strLocalIp = conf.GetValue("global", "connector_ext_ip", "127.0.0.1");
        strHttpIp = conf.GetValue("resetful", "http_ext_ip", "127.0.0.1");
        strRedisIp1 = conf.GetValue("redis", "redis_ext_ip1", "127.0.0.1");
    }
    if (strLocalIp.length()==0 || strHttpIp.length()==0 || strRedisIp1.length()==0) {
        std::cout << "Error: Ip length is 0!" << std::endl;
        std::cout << "Please enter any key to exit ..." << std::endl;
        getchar();
        exit(0);
    }

    int nWebConPort = conf.GetIntVal("global", "listen_webcon_port", 6610);
    int nModulePort = conf.GetIntVal("global", "listen_module_port", 6620);
    int nCliConPort = conf.GetIntVal("global", "listen_clicon_port", 6630);
    int nWebSvrPort = conf.GetIntVal("global", "listen_websvr_port", 6612);
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

    CRTConnManager::Instance().Init(strRedisIp1, nRedisPort1);
    std::string ssid;
    GenericSessionId(ssid);
    CRTConnManager::Instance().SetConnectorInfo(strLocalIp, nWebConPort, ssid.c_str());
    LI("[][]ConnectorId:%s\n", ssid.c_str());

	if(nWebConPort > 0)
	{
		m_pConnListener = new CRTConnListener();
		OS_Error err = m_pConnListener->Initialize(INADDR_ANY, nWebConPort);
		if (err!=OS_NoErr)
		{
			LE("CreateConnListener error port=%d \n", nWebConPort);
			delete m_pConnListener;
			m_pConnListener=NULL;

			return -1;
		}
		LI("Start Connector Conn service:(%d) ok...,socketFD:%d\n", nWebConPort, m_pConnListener->GetSocketFD());
		m_pConnListener->RequestEvent(EV_RE);
	}

    if (nModulePort > 0) {
        m_pModuleListener = new CRTModuleListener();
        OS_Error err = m_pModuleListener->Initialize(INADDR_ANY, nModulePort);
        if (err!=OS_NoErr) {
            LE("CreateModuleListener error port:%d\n", nModulePort);
            delete m_pModuleListener;
            m_pModuleListener = NULL;
            return -1;
        }
        LI("Start Connector Module service:(%d) ok...,socketFD:%d\n", nModulePort, m_pModuleListener->GetSocketFD());
        m_pModuleListener->RequestEvent(EV_RE);
    }

    if (nCliConPort > 0) {
        m_pConnTcpListener = new CRTConnTcpListener();
        OS_Error err = m_pConnTcpListener->Initialize(INADDR_ANY, nCliConPort);
        if (err!=OS_NoErr) {
            LE("CreateConnTcpListener error port:%d\n", nCliConPort);
            delete m_pConnTcpListener;
            m_pConnTcpListener = NULL;
            return -1;
        }
        LI("Start Connector ConnTcp service:(%d) ok...,socketFD:%d\n", nCliConPort, m_pConnTcpListener->GetSocketFD());
        m_pConnTcpListener->RequestEvent(EV_RE);
    }

#if 0
    if(nWebSvrPort > 0)
	{
		m_pWebSvrListener = new CRTWebServerListener();
		OS_Error err = m_pWebSvrListener->Initialize(INADDR_ANY, nWebSvrPort);
		if (err!=OS_NoErr)
		{
			LE("Create WebServerListener error port=%d \n", nWebSvrPort);
			delete m_pWebSvrListener;
			m_pWebSvrListener=NULL;

			return -1;
		}
		LI("Start Connector WebServer service:(%d) ok...,socketFD:%d\n", nWebSvrPort, m_pWebSvrListener->GetSocketFD());
		m_pWebSvrListener->RequestEvent(EV_RE);
	}
#endif

	return 0;
}

void CRTConnector::DoTick()
{
#if 0

#endif
}

void CRTConnector::Stop()
{
    CRTConnManager::Instance().SignalKill();
    CRTConnManager::Instance().ClearAll();
    CRTConnManager::Instance().Unin();
}
