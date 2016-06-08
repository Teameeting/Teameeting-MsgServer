#include "RTTcpNoTimeout.h"
#include "RTJSBuffer.h"
#include "atomic.h"

unsigned int RTTcpNoTimeout::sSessionIDCounterNo = kFirstTCPSessionID;
RTTcpNoTimeout::RTTcpNoTimeout()
: Task()
, fTimeoutTask(NULL, 120 * 1000)
, fSocket(NULL, Socket::kNonBlockingSocketType)
, fTickTime(0)
{
	ListZero(&m_listSend);

	fSessionID = (UInt32)atomic_add(&sSessionIDCounterNo, 1);

	fTimeoutTask.SetTask(this);
    fSocket.SetTask(this);
}

RTTcpNoTimeout::~RTTcpNoTimeout(void)
{
	ListEmpty(&m_listSend);
    GetSocket()->Cleanup();
}

int RTTcpNoTimeout::SendData(const char*pData, int nLen)
{
    if (nLen > 9999) {
        LE("RTTcpNoTimeout::SendData pData is over length\n");
        return -1;
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+1));
        memcpy(ptr, pData, nLen);
        ptr[nLen] = '\0';
        {
            OSMutexLocker locker(&mMutexSend);
            ListAppend(&m_listSend, ptr, nLen);
        }
    }

	this->Signal(kWriteEvent);
	return nLen;
}

int RTTcpNoTimeout::SendTransferData(const char*pData, int nLen)
{
    if (nLen > 9999) {
        LE("RTTcpNoTimeout::SendTransferData pData is over length\n");
        return -1;
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+4));
        char* pptr = ptr;
        *pptr = '$';
        (pptr)++;
        RTJSBuffer::writeShort(&pptr, nLen);
        memcpy((pptr), pData, nLen);
        ptr[nLen+3] = '\0';
        {
#if 0
            OSMutexLocker locker(&mMutexSend);
            ListAppend(&m_listSend, ptr, nLen+3);
#else
        std::string str(ptr, nLen+3);
        {
            OSMutexLocker locker(&mMutexSend);
            m_dequeSend.push_back(str);
        }
        free(ptr);
        ptr = NULL;
#endif
        }
        pptr = NULL;
    }

    this->Signal(kWriteEvent);
    return nLen;
}

SInt64 RTTcpNoTimeout::Run()
{
	EventFlags events = this->GetEvents();
	this->ForceSameThread();

	// Http session is short connection, need to kill session when occur TimeoutEvent.
	// So return -1.
    if (events&Task::kTimeoutEvent)
    {
        printf("RTTcpNoTimeout checked timeout here!!!\n");
        UpdateTimer();
        return 0;
    }
	if(events&Task::kKillEvent)
	{
        ObserverConnectionMapIt it = m_mapConnectObserver.find(this);
        if (it != m_mapConnectObserver.end()) {
            RTObserverConnection *conn = it->second;
            if (conn) {
                conn->ConnectionDisconnected();
            }
        }
        printf("RTTcpNoTimeout timeout or killed\n");
		return -1;
	}

	while(this->IsLiveSession())
	{
		if(events&Task::kReadEvent)
		{
			UInt32	readed = 0;
			char	fRequestBuffer[kRequestBufferSizeInBytes];
			while(1)
			{
				readed = 0;
				// We don't have any new data, get some from the socket...
				OS_Error sockErr = fSocket.Read(fRequestBuffer, kRequestBufferSizeInBytes - 1, &readed);
				if (sockErr == EAGAIN)
					break;
				if (sockErr != OS_NoErr)
				{
					Assert(!fSocket.IsConnected());
					break;
				}
				if(readed > 0)
				{
                    //(fRequestBuffer[0] != '$')
					OnRecvData(fRequestBuffer, readed);
				}
			}

			fSocket.RequestEvent(EV_RE);
			events -= Task::kReadEvent;
		}
		else if(events&Task::kWriteEvent)
		{
#if 0
			ListElement *elem = NULL;
			if((elem = m_listSend.first) != NULL)
			{
				UInt32 theLengthSent = 0;
				OS_Error err = fSocket.Send((char*)elem->content, elem->size, &theLengthSent);
				if (err == EAGAIN)
				{
					fSocket.RequestEvent(EV_RE | EV_WR);
				}
				else
				{
                    {
                        OSMutexLocker locker(&mMutexSend);
                        printf("RTTcpNoTimeout::Run kWriteEvent m_listSend.size:%d, m_listSend.count:%d\n", m_listSend.size, m_listSend.count);
                        ListRemoveHead(&m_listSend);
                    }
					if(NULL != m_listSend.first)
						this->Signal(kWriteEvent);
				}
			}
            //OnSendEvent("", 0);
			events -= Task::kWriteEvent;
#else
            if (m_dequeSend.size()>0)
            {
                std::string s = m_dequeSend.front();
				UInt32 theLengthSent = 0;
				OS_Error err = fSocket.Send(s.c_str(), s.length(), &theLengthSent);
				if (err == EAGAIN)
				{
					fSocket.RequestEvent(EV_RE | EV_WR);
				}
				else
				{
                    {
                        OSMutexLocker locker(&mMutexSend);
                        m_dequeSend.pop_front();
                    }
					if(m_dequeSend.size()>0)
						this->Signal(kWriteEvent);
				}
            }
			events -= Task::kWriteEvent;
#endif
		}
		else if(events&Task::kWakeupEvent)
		{
			OnWakeupEvent("", 0);
			events -= Task::kWakeupEvent;
		}
		else if(events&Task::kPushEvent)
		{
			OnPushEvent("", 0);
			events -= Task::kPushEvent;
		}
		else if(events&Task::kIdleEvent)
		{
			//OnTickEvent("", 0);
			events -= Task::kIdleEvent;
		}
		else
		{
			return fTickTime;
		}
	}

	// If we are here because of a timeout, but we can't delete because someone
    // is holding onto a reference to this session, just reschedule the timeout.
    //
    // At this point, however, the session is DEAD.
    ObserverConnectionMapIt it = m_mapConnectObserver.find(this);
    if (it != m_mapConnectObserver.end()) {
        RTObserverConnection *conn = it->second;
        if (conn) {
            conn->ConnectionDisconnected();
        }
    }
    printf("RTTcpNoTimeout session die...\n");
    return -1;
}

void RTTcpNoTimeout::AddObserver(RTObserverConnection* conn)
{
    m_OCMItPair = m_mapConnectObserver.insert(std::make_pair(this, conn));
    if (!m_OCMItPair.second) {
        m_mapConnectObserver.erase(this);
        m_mapConnectObserver.insert(std::make_pair(this, conn));
    }
}

void RTTcpNoTimeout::DelObserver(RTObserverConnection* conn)
{
    m_mapConnectObserver.erase(this);
}
