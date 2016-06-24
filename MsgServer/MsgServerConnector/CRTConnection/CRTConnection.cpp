#include "CRTConnection.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "CRTConnManager.h"
#include "RTUtils.hpp"

#define DEF_PTORO 1
#include "ProtoCommon.h"

std::string		CRTConnection::gStrAddr;
unsigned short	CRTConnection::gUsPort = 0;
CRTConnection::CRTConnection(void)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
{
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
    AddObserver(this);
}

CRTConnection::~CRTConnection(void)
{
    DelObserver(this);
	delete[] m_pBuffer;
}

int CRTConnection::SendDispatch(const std::string& id, const std::string& msg)
{
    OSMutexLocker  locker(&m_mutex);
    CRTConnHttp::SendResponse(HPS_OK, msg);
    return 0;
}


//* For RCTcp
void CRTConnection::OnRecvData(const char*data, int size)
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
		parsed = CRTConnHttp::ProcessData(m_pBuffer, m_nBufOffset);

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
void CRTConnection::OnLogin(const char* pUserid, const char* pPass, const char* pNname)
{
    bool ok = false;
    {
        //check & auth
#if 0

#endif
        m_userId = pUserid;
        m_token = pPass;
        m_nname = pNname;
    }
    //if (ok) {
    if (false) {
        std::string sid;
        //store userid & pass
        CRTConnManager::ConnectionInfo* pci = new CRTConnManager::ConnectionInfo();
        if (pci) {
            GenericSessionId(sid);
            m_connectorId = CRTConnManager::Instance().ConnectorId();
            pci->_connId = sid;
            pci->_connAddr = CRTConnManager::Instance().ConnectorIp();
            pci->_connPort = CRTConnManager::Instance().ConnectorPort();
            pci->_userId = pUserid;
            pci->_token = pPass;
            pci->_pConn = NULL;
            pci->_connType = pms::EConnType::THTTP;
            pci->_flag = 1;
            std::string uid(pUserid);
            CRTConnManager::Instance().AddUser(pms::EConnType::THTTP, uid, pci);
        } else {
            LE("new ConnectionInfo error!!!\n");
        }
    } else {
        std::string uid(pUserid);
        CRTConnManager::ConnectionInfo* pci = NULL;
        CRTConnManager::Instance().AddUser(pms::EConnType::THTTP, uid, pci);
    }
    {
        // send response
        rapidjson::Document     jsonDoc;
        rapidjson::StringBuffer jsonStr;
        rapidjson::Writer<rapidjson::StringBuffer>   jsonWriter(jsonStr);
        jsonDoc.SetObject();
        if (ok) {
            jsonDoc.AddMember("login", "success", jsonDoc.GetAllocator());
        } else {
            jsonDoc.AddMember("login", "failed", jsonDoc.GetAllocator());
        }
        jsonDoc.Accept(jsonWriter);
        SendResponse(HPS_OK, jsonStr.GetString());
    }
}

void CRTConnection::OnSndMsg(const char* pUserid, int mType, const char* pData, int dLen)
{
    if (!pData) {
        LE("%s invalid params\n", __FUNCTION__);
        SendResponse(HPS_BAD_REQUEST, "");
        return;
    }

    //no matter mType is meet or callcenter or p2p or others
    //the following code should be same
    //find an TrasnferSession By mtype
    //transfer msg by TransferSession

}

void CRTConnection::OnGetMsg(const char* pUserid, int mType)
{
    if (!pUserid) {
        LE("%s invalid params\n", __FUNCTION__);
        SendResponse(HPS_BAD_REQUEST, "");
        return;
    }
    {
        //get msg
        RTTcp::UpdateTimer();
    }
}

void CRTConnection::OnLogout(const char* pUserid)
{

}

void CRTConnection::OnResponse(const char*pData, int nLen)
{
	RTTcp::SendData(pData, nLen);
}

void CRTConnection::ConnectionDisconnected()
{
    if (m_userId.length()) {
        std::string token;
        CRTConnManager::Instance().DelUser(pms::EConnType::THTTP, m_userId, token);
        CRTConnManager::Instance().ConnectionLostNotify(m_userId, m_token);
    } else {
        LE("RTConnection::ConnectionDisconnected m_userId.length is 0\n");
    }
}

////////////////////////////////////////////////
///////////////////private//////////////////////
////////////////////////////////////////////////

