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
#include "CRTConnectionManager.h"
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

#if !MACOSXEVENTQUEUE
	::select_stopevents();
#endif

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
{

}

CRTConnector::~CRTConnector(void)
{
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

int	CRTConnector::Start(const char*pWebConIp, unsigned short usWebConPort
                  , const char*pModuleIp, unsigned short usModulePort
                  , const char*pCliConIp, unsigned short usCliConPort
                  , const char*pHttpIp, unsigned short usHttpPort)
{
	Assert(g_inited);
	Assert(pWebConIp != NULL && strlen(pWebConIp)>0);
	Assert(pModuleIp != NULL && strlen(pModuleIp)>0);
    Assert(pCliConIp != NULL && strlen(pCliConIp)>0);
    Assert(pHttpIp != NULL && strlen(pHttpIp)>0);

    char hh[24] = {0};
    sprintf(hh, "%s:%u", pHttpIp, usHttpPort);

    CRTConnectionManager::s_cohttpHost = hh;
    CRTConnectionManager::s_cohttpIp = pHttpIp;
    CRTConnectionManager::s_cohttpPort = usHttpPort;
    
    std::string ssid;
	CRTConnection::gStrAddr = pWebConIp;
    CRTConnection::gUsPort = usWebConPort;
    CRTDispatchConnection::m_connIp = pWebConIp;
    CRTDispatchConnection::m_connPort = usWebConPort;
    GenericSessionId(ssid);
    CRTConnectionManager::Instance()->SetConnectorInfo(pWebConIp, usWebConPort, ssid.c_str());
    LI("[][]ConnectorId:%s\n", ssid.c_str());

	if(usWebConPort == 0)
	{
		LE("Connector server conn need ...!");
		Assert(false);
	}

	if(usWebConPort > 0)
	{
		m_pConnListener = new CRTConnListener();
		OS_Error err = m_pConnListener->Initialize(INADDR_ANY, usWebConPort);
		if (err!=OS_NoErr)
		{
			LE("CreateConnListener error port=%d \n", usWebConPort);
			delete m_pConnListener;
			m_pConnListener=NULL;

			return -1;
		}
		LI("Start Connector Conn service:(%d) ok...,socketFD:%d\n", usWebConPort, m_pConnListener->GetSocketFD());
		m_pConnListener->RequestEvent(EV_RE);
	}

    if (usModulePort == 0) {
        LE("Connector server meet need ...!!");
        Assert(false);
    }

    if (usModulePort > 0) {
        m_pModuleListener = new CRTModuleListener();
        OS_Error err = m_pModuleListener->Initialize(INADDR_ANY, usModulePort);
        if (err!=OS_NoErr) {
            LE("CreateModuleListener error port:%d\n", usModulePort);
            delete m_pModuleListener;
            m_pModuleListener = NULL;
            return -1;
        }
        LI("Start Connector Module service:(%d) ok...,socketFD:%d\n", usModulePort, m_pModuleListener->GetSocketFD());
        m_pModuleListener->RequestEvent(EV_RE);
    }

    if (usCliConPort == 0) {
        LE("Connector server meet need ...!!!!");
        Assert(false);
    }
    if (usCliConPort > 0) {
        m_pConnTcpListener = new CRTConnTcpListener();
        OS_Error err = m_pConnTcpListener->Initialize(INADDR_ANY, usCliConPort);
        if (err!=OS_NoErr) {
            LE("CreateConnTcpListener error port:%d\n", usCliConPort);
            delete m_pConnTcpListener;
            m_pConnTcpListener = NULL;
            return -1;
        }
        LI("Start Connector ConnTcp service:(%d) ok...,socketFD:%d\n", usCliConPort, m_pConnTcpListener->GetSocketFD());
        m_pConnTcpListener->RequestEvent(EV_RE);
    }
    if (usHttpPort == 0) {
        LE("Connector server meet need ...!!!!");
        Assert(false);
    }
    if (usHttpPort > 0) {
        LI("Start Connector Http service:(%d) ok...\n", usHttpPort);
    }
    
	return 0;
}

void CRTConnector::DoTick()
{
#if 0

#endif
}

void CRTConnector::Stop()
{

}
