#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "DRTMsgQueue.h"
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
#include "DRTConnectionManager.h"
#include "RTHiredis.h"


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

#if !MACOSXEVENTQUEUE
	::select_stopevents();
#endif

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

int	DRTMsgQueue::Start(const char*pConnIp, unsigned short usConnPort, const char*pModuleIp, unsigned short usModulePort)
{
	Assert(g_inited);
	Assert(pConnIp != NULL && strlen(pConnIp)>0);
	Assert(pModuleIp != NULL && strlen(pModuleIp)>0);
    
    RTHiredisLocal::Instance()->Connect();
    std::string mid;
    DRTConnectionManager::Instance()->GenericSessionId(mid);
    DRTConnectionManager::Instance()->SetMsgQueueId(mid);
    LI("[][]MsgQueueId:%s\n", mid.c_str());
    
	if(usConnPort == 0)
	{
		LE("MsgQueue server need usConnPort...!");
		Assert(false);
	}

	if(usConnPort > 0)
	{
        char addr[24] = {0};
        sprintf(addr, "%s %u", pConnIp, usConnPort);
        DRTConnectionManager::Instance()->GetAddrsList()->push_front(addr);
        
        if (!(DRTConnectionManager::Instance()->ConnectConnector())) {
            LE("Start to ConnectConnector failed\n");
            return -1;
        }
	}
    
    if(usModulePort == 0)
	{
		LE("MsgQueue server need usModulePort...!!");
		Assert(false);
	}

	if(usModulePort > 0)
	{
        m_pModuleListener = new DRTModuleListener();
        OS_Error err = m_pModuleListener->Initialize(INADDR_ANY, usModulePort);
        if (err!=OS_NoErr)
        {
            LE("CreateMeetingListener error port=%d \n", usModulePort);
            delete m_pModuleListener;
            m_pModuleListener=NULL;
            return -1;
        }
        LI("Start MsgQueue service meet:(%d) ok...,socketFD:%d\n", usModulePort, m_pModuleListener->GetSocketFD());
        m_pModuleListener->RequestEvent(EV_RE);
	}
    
   return 0;
}

void DRTMsgQueue::DoTick()
{
    DRTConnectionManager::Instance()->RefreshConnection();
#if 0
    std::list<const std::string> ks;
    std::list<const std::string> vs;
    RTHiredis::Instance()->CmdHLen(HI_USER_CONNECTOR_ID, NULL);
    RTHiredis::Instance()->CmdHKeys(HI_USER_CONNECTOR_ID, &ks);
    RTHiredis::Instance()->CmdHVals(HI_USER_CONNECTOR_ID, &vs);
    std::list<const std::string>::iterator ki = ks.begin();
    std::list<const std::string>::iterator vi = vs.begin();
    for (; ki!=ks.end(); ki++) {
        LI("ki:%s\n", (*ki).c_str());
    }
    for (; vi!=vs.end(); vi++) {
        LI("vi:%s\n", (*vi).c_str());
    }
#endif
}

void DRTMsgQueue::Stop()
{

}

