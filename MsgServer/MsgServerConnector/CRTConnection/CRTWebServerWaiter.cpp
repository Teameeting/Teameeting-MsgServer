#include "CRTWebServerWaiter.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "CRTConnManager.h"
#include "RTUtils.hpp"
#include "CRTConnection.h"
#include "CRTConnectionTcp.h"

#define DEF_PTORO 1
#include "ProtoCommon.h"

CRTWebServerWaiter::CRTWebServerWaiter(void)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
{
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
    AddObserver(this);
    LI("CRTWebServerWaiter::CRTWebServerWaiter\n");
}

CRTWebServerWaiter::~CRTWebServerWaiter(void)
{
    LI("CRTWebServerWaiter::~~~~~~CRTWebServerWaiter\n");
    DelObserver(this);
	delete[] m_pBuffer;
}

int CRTWebServerWaiter::SendDispatch(const std::string& id, const std::string& msg)
{
    OSMutexLocker  locker(&m_mutex);
    CRTWebServerHttp::SendResponse(HPS_OK, msg);
    return 0;
}


//* For RCTcp
void CRTWebServerWaiter::OnRecvData(const char*data, int size)
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
		parsed = CRTWebServerHttp::ProcessData(m_pBuffer, m_nBufOffset);

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
void CRTWebServerWaiter::OnOtherLogin(const char* pContent)
{
    // check null and limit length 256
    if (!pContent || strlen(pContent)>256) return;
    LI("CRTWebServerWaiter::OnOtherLogin was called, pContent:%s\n", pContent);
    std::string cont(pContent);
    std::size_t found = cont.find_first_of("=");
    if (found != std::string::npos) {
        std::string key = cont.substr(0, found);
        std::string val = cont.substr (found+1);
        if (key.compare("userid")!=0 || val.length()==0) return;

        CRTConnManager::ConnectionInfo* pci = CRTConnManager::Instance().findConnectionInfoById(val);
        if (pci) {
            if (pci->_pConn && pci->_pConn->IsLiveSession()) {
                if (pci->_connType == pms::EConnType::THTTP) {
                    CRTConnection *c = dynamic_cast<CRTConnection*>(pci->_pConn);
                    if (c) {
                        //c->NotifyOtherLogin(val);
                    }
                } else if (pci->_connType == pms::EConnType::TTCP) {
                    CRTConnectionTcp *ct = dynamic_cast<CRTConnectionTcp*>(pci->_pConn);
                    if (ct) {
                        //ct->NotifyOtherLogin(val);
                    }
                }
            }
        }
    }
}

void CRTWebServerWaiter::OnResponse(const char*pData, int nLen)
{
	RTTcp::SendData(pData, nLen);
}

void CRTWebServerWaiter::ConnectionDisconnected()
{
    LI("CRTWebServerWaiter::ConnectionDisconnected was called\n");
}

////////////////////////////////////////////////
///////////////////private//////////////////////
////////////////////////////////////////////////

