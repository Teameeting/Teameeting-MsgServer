#include "RTConnection.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"
#include "RTConnectionManager.h"
#include "RTMessage.h"
#include "atomic.h"
#include "RTHiredis.h"

static unsigned int	g_trans_id = 0;

std::string		RTConnection::gStrAddr;
unsigned short	RTConnection::gUsPort = 0;
RTConnection::RTConnection(void)
: m_pBuffer(NULL)
, m_nBufLen(0)
, m_nBufOffset(0)
{
	m_nBufLen = kRequestBufferSizeInBytes;
	m_pBuffer = new char[m_nBufLen];
    AddObserver(this);
}

RTConnection::~RTConnection(void)
{
    DelObserver(this);
	delete[] m_pBuffer;
}

int RTConnection::SendDispatch(const std::string& id, const std::string& msg)
{
    OSMutexLocker  locker(&m_mutex);
    RTConnHttp::SendResponse(HPS_OK, msg);
    return 0;
}


//* For RCTcp
void RTConnection::OnRecvData(const char*pData, int nLen)
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

void RTConnection::OnLcsEvent()
{

}

//* For RTConnHttp
void RTConnection::OnLogin(const char* pUserid, const char* pPass)
{
    bool ok = false;
    {
        //check & auth
        //pPass is token, read token from redis
        //get userid compare userid and pUserid
        //exist ok, not exist not ok
        
#if 0
        std::string pass;
        RTHiredisRemote::Instance()->CmdGet(pUserid, pass);
        if (pass.compare(pPass)!=0) {
            LE("OnLogin user pass has error, pUserid:%s, pPass:%s, pass:%s\n", pUserid, pPass, pass.c_str());
            return;
        }
        m_userId = pUserid;
        m_token = pPass;
#endif
    }
    //if (ok) {
    if (false) {
        std::string sid;
        //store userid & pass
        RTConnectionManager::ConnectionInfo* pci = new RTConnectionManager::ConnectionInfo();
        if (pci) {
            RTConnectionManager::Instance()->GenericSessionId(sid);
            m_connectorId = RTConnectionManager::Instance()->ConnectorId();
            pci->connId = sid;
            pci->connAddr = RTConnectionManager::Instance()->ConnectorIp();
            pci->connPort = RTConnectionManager::Instance()->ConnectorPort();
            pci->userId = pUserid;
            //pci->pConn = this;
            pci->pConn = NULL;
            pci->connType = CONNECTIONTYPE::_chttp;
            pci->flag = 1;
            std::string uid(pUserid);
            RTConnectionManager::Instance()->AddUser(CONNECTIONTYPE::_chttp, uid, pci);
        } else {
            LE("new ConnectionInfo error!!!\n");
        }
    } else {
        std::string uid(pUserid);
        RTConnectionManager::ConnectionInfo* pci = NULL;
        RTConnectionManager::Instance()->AddUser(CONNECTIONTYPE::_chttp, uid, pci);
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

void RTConnection::OnSndMsg(const char* pUserid, int mType, const char* pData, int dLen)
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

    TRANSFERMSG t_trmsg;
    TRANSMSG t_msg;
    t_msg._flag = 0;
    t_msg._touser = "";
    t_msg._connector = RTConnectionManager::Instance()->ConnectorId();
    t_msg._content = pData;
    
    t_trmsg._action = TRANSFERACTION::req;
    t_trmsg._fmodule = TRANSFERMODULE::mconnector;
    t_trmsg._type = TRANSFERTYPE::trans;
    t_trmsg._trans_seq = GenericTransSeq();
    t_trmsg._trans_seq_ack = 0;
    t_trmsg._valid = 1;
    t_trmsg._content = t_msg.ToJson();
    
    const std::string s = t_trmsg.ToJson();
    RTConnectionManager::ModuleInfo* pmi = RTConnectionManager::Instance()->findModuleInfo(pUserid, (TRANSFERMODULE)mType);
    if (pmi && pmi->pModule) {
        pmi->pModule->SendTransferData(s.c_str(), (int)s.length());
    } else {
        LE("pmi->pModule is NULL\n");
    }
    SendResponse(HPS_OK, "");
}

void RTConnection::OnGetMsg(const char* pUserid, int mType)
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
    {
        //send back to user
        rapidjson::Document      jsonDoc;
        rapidjson::StringBuffer  jsonStr;
        rapidjson::Writer<rapidjson::StringBuffer>   jsonWriter(jsonStr);
        jsonDoc.SetObject();
        
        jsonDoc.AddMember("getmsg", "success", jsonDoc.GetAllocator());
        jsonDoc.Accept(jsonWriter);
        
        SendResponse(HPS_OK, jsonStr.GetString());
        
    }
}

void RTConnection::OnLogout(const char* pUserid)
{
    {
        //logout
        //write to hiredis
        std::string uid(pUserid);
        RTConnectionManager::Instance()->DelUser(CONNECTIONTYPE::_chttp, uid);
        m_userId.assign("");
        m_token.assign("");
    }
    {
        //send response
        rapidjson::Document       jsonDoc;
        rapidjson::StringBuffer   jsonStr;
        rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
        jsonDoc.SetObject();
        
        jsonDoc.AddMember("logout", "success", jsonDoc.GetAllocator());
        jsonDoc.Accept(jsonWriter);
        
        SendResponse(HPS_OK, jsonStr.GetString());
    }
}

void RTConnection::OnResponse(const char*pData, int nLen)
{
	RTTcp::SendData(pData, nLen);
}

void RTConnection::ConnectionDisconnected()
{
    if (m_userId.length()) {
        LI("RTConnection::ConnectionDisconnected DelUser m_userId:%s, m_token:%s\n", m_userId.c_str(), m_token.c_str());
        RTConnectionManager::Instance()->ConnectionLostNotify(m_userId, m_token);
    } else {
        LE("RTConnection::ConnectionDisconnected m_userId.length is 0\n");
    }
}

////////////////////////////////////////////////
///////////////////private//////////////////////
////////////////////////////////////////////////

int RTConnection::GenericTransSeq()
{
    return atomic_add(&g_trans_id, 1);
}