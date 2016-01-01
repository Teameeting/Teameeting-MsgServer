#include "XTcpTick.h"
#include <map>

#ifdef WIN32
#include "webrtc/base/win32socketserver.h"
#else
#include <unistd.h>
#endif

namespace rtc {
class XTcpMgr
		: public Thread
	{
	public:
		XTcpMgr() 
			: m_bRunning(false)
			, m_curThread(NULL){
		};
		virtual ~XTcpMgr(){
		};

	public:
		virtual void AddTick(XTcpTick*pTick){
			if (pTick == NULL)
				return;

			CritScope cs(&m_csTick);
			m_mapTick[pTick] = pTick;
			if (m_mapTick.size() == 1 && !m_bRunning)
			{
				m_bRunning = true;
				this->Start();
			}

		};

		virtual void RemoveTick(XTcpTick*pTick){
			if (pTick == NULL)
				return;

			int size = 0;
			{
				CritScope cs(&m_csTick);
				m_mapTick.erase(pTick);
				pTick->OnFinish();
				size = m_mapTick.size();
			}
			
			if (size == 0 && m_bRunning)
			{
				m_bRunning = false;
				this->Stop();
			}
		};

	public:
		//* For Thread
		virtual void Run(){
#if WIN32
			// Need to pump messages on our main thread on Windows.
			Win32Thread w32_thread;
			rtc::ThreadManager::Instance()->SetCurrentThread(&w32_thread);
#endif
			m_curThread = rtc::Thread::Current();
			while (m_bRunning)
			{
				{// ProcessMessages
					if (!this->IsCurrent())
						this->ProcessMessages(1);

					m_curThread->ProcessMessages(1);
				}
				
				{// OnTick
					CritScope cs(&m_csTick);
					MapTick::iterator iter = m_mapTick.begin();
					while (iter != m_mapTick.end())
					{
						iter->second->OnTick();
						iter++;
					}
				}
#ifndef WIN32
				usleep(1000);
#endif
			}
		};

	private:
		bool				m_bRunning;
		Thread*				m_curThread;
		CriticalSection		m_csTick;
		typedef std::map<void*, XTcpTick*> MapTick;
		MapTick				m_mapTick;
	};
};

static rtc::XTcpMgr g_XTcpMgr;

XTcpTick::XTcpTick(void)
{
	m_pThread = &g_XTcpMgr;
}
XTcpTick::~XTcpTick(void)
{
	m_pThread = NULL;
}

int XTcpTick::Open(void)
{
	g_XTcpMgr.AddTick(this);
	return 0;
}

void XTcpTick::Close(void)
{
	g_XTcpMgr.RemoveTick(this);
}

rtc::Thread* XTcpTick::CurThread()
{
	return m_pThread; 
}


