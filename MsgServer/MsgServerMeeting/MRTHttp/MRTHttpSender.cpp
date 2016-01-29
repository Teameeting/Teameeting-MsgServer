#include <stdio.h>
#include "SocketUtils.h"
#include "MRTHttpSender.h"
#include "MRTRoomManager.h"

#define HTTP_SENDER_TIMEOUT (60000)

MRTHttpSender::MRTHttpSender(void)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
, m_method(HTTP_POST)
, m_cmd(_http_cmd_invalid)
, m_transmsg()
, m_meetmsg()
{
    SetTimer(HTTP_SENDER_TIMEOUT);
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
}

MRTHttpSender::MRTHttpSender(HTTPCMD cmd, TRANSMSG& tmsg, MEETMSG& msg)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
, m_method(HTTP_GET)
, m_cmd(cmd)
, m_transmsg(tmsg)
, m_meetmsg(msg)
{
    SetTimer(HTTP_SENDER_TIMEOUT);
}

MRTHttpSender::~MRTHttpSender(void)
{
	delete[] m_pBuffer;
    m_pBuffer = NULL;
}

bool MRTHttpSender::ConnHttpHost(const std::string& addr, const unsigned short port, const std::string& host)
{
    SetHttpHost(host);
    TCPSocket* socket = this->GetSocket();
    if (socket->Open() != OS_NoErr) {
        LE("%s socket->Open failed!!!\n", __FUNCTION__);
        return false;
    }
    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->SetSocketBufSize(10L * 1024L);//10k
    socket->SetTask(this);
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
void MRTHttpSender::OnRecvData(const char*data, int size)
{
	{//
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

	{//
		int parsed = 0;
		parsed = MRTConnHttp::ProcessData(m_pBuffer, m_nBufOffset);

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

//* For RTConnHttp

void MRTHttpSender::OnResponse(const char*pData, int nLen)
{
    if (!pData || nLen<=0) {
        LE("RTHttpSender::OnResponse pData nLen error\n");
        return;
    }
    if (m_method == HTTP_GET) {
        std::string data(pData, nLen);
        MRTRoomManager::Instance()->HandleOptRoomWithData(m_cmd, m_transmsg, m_meetmsg, data);
    } else {
        LI("OnResponse recieved, kill event\n");
        this->Signal(kKillEvent);
    }

}


void MRTHttpSender::SendRequest(const char* pData, int nLen)
{
    RTTcp::SendData(pData, nLen);
    if  (GetSocket()) {
        GetSocket()->RequestEvent(EV_RE);
    }
}
