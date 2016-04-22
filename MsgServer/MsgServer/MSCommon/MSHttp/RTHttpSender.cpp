#include <stdio.h>
#include "SocketUtils.h"
#include "RTHttpSender.h"


#define HTTP_SENDER_TIMEOUT (60000)

RTHttpSender::RTHttpSender(void)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
, m_method(HTTP_POST)
, m_cmd(0)
, m_transmsg()
, m_meetmsg()
{
    SetTimer(HTTP_SENDER_TIMEOUT);
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
}

RTHttpSender::RTHttpSender(int cmd, pms::RelayMsg& rmsg, pms::MeetMsg& msg)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
, m_method(HTTP_GET)
, m_cmd(cmd)
, m_transmsg(rmsg)
, m_meetmsg(msg)
{
    SetTimer(HTTP_SENDER_TIMEOUT);
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
        LE("RTHttpSender::ConnHttpHost socket->Open failed!!!\n");
        return false;
    }
    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->SetSocketBufSize(10L * 1024L);//10k
    socket->SetTask(this);
    OS_Error err;
    do{
        usleep(50*1000);
        err = socket->Connect(SocketUtils::ConvertStringToAddr(addr.c_str()), port);
    }while(!(err==OS_NoErr || err==EISCONN));
    //socket->RequestEvent(EV_RE);
    return true;
}

////////////////////////////////////////////////////

//* For RCTcp
void RTHttpSender::OnRecvData(const char*data, int size)
{
	{
        while ((m_nBufOffset + size) > m_nBufLen)
        {
            int newLen = m_nBufLen + kRequestBufferSizeInBytes;
            if (size > newLen)
                newLen = m_nBufLen + size;
            char* temp = new char[newLen];
            if (temp == NULL)
                continue;
            memcpy(temp, m_pBuffer, m_nBufLen);
            delete[] m_pBuffer;
            m_pBuffer = temp;
            m_nBufLen = newLen;
        }

        memcpy(m_pBuffer + m_nBufOffset, data, size);
        m_nBufOffset += size;
	}

	{
		int parsed = 0;
		parsed = RTConnHttp::ProcessData(m_pBuffer, m_nBufOffset);
        if (parsed > 0 && m_pBuffer != NULL)
        {
            m_nBufOffset -= parsed;
            if (m_nBufOffset == 0)
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

void RTHttpSender::SendRequest(const char* pData, int nLen)
{
    RTTcp::SendData(pData, nLen);
    if  (GetSocket()) {
        GetSocket()->RequestEvent(EV_RE);
    }
}
