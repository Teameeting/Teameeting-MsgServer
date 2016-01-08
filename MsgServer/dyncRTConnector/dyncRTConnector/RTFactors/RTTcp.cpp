#include "RTTcp.h"
#include "atomic.h"

unsigned int RTTcp::sSessionIDCounter = kFirstTCPSessionID;
RTTcp::RTTcp()
: Task()
, fTimeoutTask(NULL, 60 * 1000)
, fSocket(NULL, Socket::kNonBlockingSocketType)
, fTickTime(0)
{
	ListZero(&m_listSend);

	fSessionID = (UInt32)atomic_add(&sSessionIDCounter, 1);

	fTimeoutTask.SetTask(this);
    fSocket.SetTask(this);
}

RTTcp::~RTTcp(void)
{
	ListEmpty(&m_listSend);
}

int RTTcp::SendData(const char*pData, int nLen)
{
    if (nLen > 9999) {
        LE("%s invalid params\n", __FUNCTION__);
        return -1;
    }
    {
        char* ptr = new char[nLen];
        memcpy(ptr, pData, nLen);
        ListAppend(&m_listSend, ptr, nLen);
    }

	this->Signal(kWriteEvent);
	return nLen;
}

int RTTcp::SendTransferData(const char*pData, int nLen)
{
    if (nLen > 9999) {
        LE("%s invalid params\n", __FUNCTION__);
        return -1;
    }
    {
        char* ptr = new char[nLen+6];//sprintf will add 1 in the end
        sprintf(ptr, "$%4d%s", nLen, pData);
        ListAppend(&m_listSend, ptr, nLen+5);
    }
    
    this->Signal(kWriteEvent);
    return nLen;
}

SInt64 RTTcp::Run()
{
	EventFlags events = this->GetEvents();
	this->ForceSameThread();

	// Http session is short connection, need to kill session when occur TimeoutEvent.
	// So return -1.
	if(events&Task::kTimeoutEvent || events&Task::kKillEvent)
	{
        if (events&Task::kTimeoutEvent) {
            LI("%s timeout \n", __FUNCTION__);
        } else {
            LI("%s kill \n", __FUNCTION__);
        }
        std::map<RTTcp*, RTObserverConnection*>::iterator it = m_mapConnectObserver.find(this);
        if (it != m_mapConnectObserver.end()) {
            LI("Tcp::Run find Disconnection\n");
            RTObserverConnection *conn = it->second;
            if (conn) {
                LI("Tcp::Run notify Disconnection\n");
                conn->ConnectionDisconnected();
            }
        }
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
					OnRecvData(fRequestBuffer, readed);
				}
			}

			fSocket.RequestEvent(EV_RE);
			events -= Task::kReadEvent; 
		}
		else if(events&Task::kWriteEvent)
		{
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
					ListRemoveHead(&m_listSend); 
					if(NULL != m_listSend.first)
						this->Signal(kWriteEvent);
				}
				
			}
			events -= Task::kWriteEvent; 
		}
		else if(events&Task::kLcsEvent)
		{
			OnLcsEvent();
			events -= Task::kLcsEvent;
		}
		else if(events&Task::kPeerEvent)
		{
			OnPeerEvent();
			events -= Task::kPeerEvent; 
		}
		else if(events&Task::kIdleEvent)
		{
			OnTickEvent();
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
    std::map<RTTcp*, RTObserverConnection*>::iterator it = m_mapConnectObserver.find(this);
    if (it != m_mapConnectObserver.end()) {
        LI("Tcp::Run SessionOffline find Disconnection\n");
        RTObserverConnection *conn = it->second;
        if (conn) {
            LI("Tcp::Run SessionOffline notify Disconnection\n");
            conn->ConnectionDisconnected();
        }
    }
    return -1;
}

void RTTcp::AddObserver(RTObserverConnection* conn)
{
    m_mapConnectObserver.insert(std::make_pair(this, conn));
}

void RTTcp::DelObserver(RTObserverConnection* conn)
{
    m_mapConnectObserver.erase(this);
}
