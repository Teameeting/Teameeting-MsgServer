#include <stdio.h>
#include "SocketUtils.h"
#include "RTHttpSender.h"
#include "RTRoomManager.h"


RTHttpSender::RTHttpSender(void)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
, m_method(HTTP_POST)
{
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
}

RTHttpSender::RTHttpSender(TRANSMSG& tmsg, MEETMSG& msg)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
, m_method(HTTP_GET)
, m_transmsg(tmsg)
, m_meetmsg(msg)
{

}

RTHttpSender::~RTHttpSender(void)
{
	delete[] m_pBuffer;
    m_pBuffer = NULL;
}

bool RTHttpSender::ConnHttpHost(const std::string& addr, const unsigned short port, const std::string& host)
{
    SetHttpHost(host);
    TCPSocket* socket = this->GetSocket();
    if (socket->Open() != OS_NoErr) {
        LE("%s socket->Open failed!!!\n", __FUNCTION__);
        return false;
    }
    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->SetSocketBufSize(96L * 1024L);
    socket->SetTask(this);
    this->SetTimer(5*1000);
    OS_Error err;
    do{
        sleep(1);
        err = socket->Connect(SocketUtils::ConvertStringToAddr(addr.c_str()), port);
        LI("Connect to http server %s:%u, waiting...\n", addr.c_str(), port);
    }while(!(err==OS_NoErr || err==EISCONN));
        
    //socket->RequestEvent(EV_RE);
    LI("%s addr:%s, port:%u\n", __FUNCTION__, addr.c_str(), port);
    return true;
}

////////////////////////////////////////////////////

//* For RCTcp
void RTHttpSender::OnRecvData(const char*pData, int nLen)
{
	{//
		while((m_nBufOffset + nLen) > m_nBufLen)
		{
			m_nBufLen += kRequestBufferSizeInBytes;
			char* ptr = (char *)realloc(m_pBuffer, m_nBufLen);
			if(ptr != NULL)
			{//
				m_pBuffer = ptr;
			}
			else
			{//
				m_nBufLen -= kRequestBufferSizeInBytes;
				continue;
			}
		}

		memcpy(m_pBuffer + m_nBufOffset, pData, nLen);
		m_nBufOffset += nLen;
	}

	{//
		int parsed = 0;
		parsed = RTConnHttp::ProcessData(m_pBuffer, m_nBufOffset);
	
		if(parsed > 0)
		{
			m_nBufOffset -= parsed;
			if(m_nBufOffset == 0)
			{
				memset(m_pBuffer, 0, m_nBufLen);
			}
			else
			{
				memmove(m_pBuffer, m_pBuffer + parsed, m_nBufOffset);
			}
		}
	}
}

//* For RTConnHttp

void RTHttpSender::OnResponse(const char*pData, int nLen)
{
    if (!pData || nLen<=0) {
        LE("RTHttpSender::OnResponse pData nLen error\n");
        return;
    }
    LI("OnResponse DataLen:%d, pData:%s\n", nLen, pData);
    if (m_method == HTTP_GET) {
        std::string data(pData, nLen);
        RTRoomManager::Instance()->HandleOptRoomWithData(m_transmsg, m_meetmsg, data);
    }
    
}


void RTHttpSender::SendRequest(const char* pData, int nLen)
{
    OSMutexLocker locker(&m_mutex);
    RTTcp::SendData(pData, nLen);
    if  (GetSocket()) {
        GetSocket()->RequestEvent(EV_RE);
    }
}
