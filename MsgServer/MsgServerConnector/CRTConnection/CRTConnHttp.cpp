#include "CRTConnHttp.h"
#include "http_excuter.h"
#include "md5digest.h"
#include "rtklog.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"

#include <stdlib.h>


long long CRTConnHttp::m_test_conn = 0;

void CRTConnHttp::OnParsedHttpMessage(const void*pUserData, http_message* httpMsg)
{
	CRTConnHttp* pHttp = (CRTConnHttp*)pUserData;
	if(pHttp)
	{
		pHttp->OnHttpMessage(httpMsg);
	}
}

CRTConnHttp::CRTConnHttp(void)
: m_bAuthed(false)
, m_nTimer(10)
, m_nHttpMethod(HTTP_POST)
{
}

CRTConnHttp::~CRTConnHttp(void)
{
}

int CRTConnHttp::ProcessData(const char*pData, int nLen)
{
	int parsed = 0;
	enum http_errno err;
	while (parsed < nLen)
	{
		const char* pMsg = pData + parsed;
		int msgLen = nLen - parsed;
		int nlen = do_http_parse(HTTP_REQUEST, (char *)pMsg, msgLen, &err, &CRTConnHttp::OnParsedHttpMessage, this);
		parsed += nlen;
		if (err != HPE_OK)
		{
			SendResponse(HPS_BAD_REQUEST, ":(");
			parsed = nLen;
			break;
		}
		else if (nlen == 0)
		{
			break;
		}
		else
		{
			//LI(">Recv:%.*s \r\n", parsed, pMsg);
		}
	}

	return parsed;
}

char* CRTConnHttp::GenerateResponse(int code, const std::string&params, const char*pData, int nDataLen, int&outLen)
{
	http_message *httpMsg = http_generator_create(HTTP_RESPONSE);
	httpMsg->status_code = (http_status)code;

	//
	http_generator_add_header(httpMsg, HPR_ACCESS_CONTROL_ALLOW_ORIGIN, "*", 1);
	
	if(params.length() > 0)
	{
		http_generator_add_header(httpMsg, HPR_PRAGMA, params.c_str(), (int)params.length());
	}

	if(pData != NULL && nDataLen > 0)
	{
		http_generator_set_content(httpMsg, pData, nDataLen, "text/json");
	}

	int nLen = 0;
	char* pMsg = http_generator_tostring(httpMsg, &nLen);
	outLen = nLen;

	http_generator_destroy(httpMsg);
	httpMsg = NULL;

	return pMsg;
}

void CRTConnHttp::SendResponse(int code, const std::string&params, const char*pData, int nDataLen)
{
	http_message *httpMsg = http_generator_create(HTTP_RESPONSE);
	httpMsg->status_code = (http_status)code;

	//
	http_generator_add_header(httpMsg, HPR_ACCESS_CONTROL_ALLOW_ORIGIN, "*", 1);
	
	if(params.length() > 0)
	{
		http_generator_add_header(httpMsg, HPR_PRAGMA, params.c_str(), (int)params.length());
	}

	if(pData != NULL && nDataLen > 0)
	{
		http_generator_set_content(httpMsg, pData, nDataLen, "text/json");
	}

	int nLen = 0;
	char* pMsg = http_generator_tostring(httpMsg, &nLen);
	
	OnResponse(pMsg, nLen);

	free(pMsg);
	http_generator_destroy(httpMsg);
	httpMsg = NULL;
}

void CRTConnHttp::SendResponse(int code, const std::string&strContent)
{
	http_message *httpMsg = http_generator_create(HTTP_RESPONSE);
	httpMsg->status_code = (http_status)code;

	//
	http_generator_add_header(httpMsg, HPR_ACCESS_CONTROL_ALLOW_ORIGIN, "*", 1);
	
	if(strContent.length() > 0)
	{
		http_generator_set_content(httpMsg, strContent.c_str(), (int)strContent.length(), "text/json");
	}

	int nLen = 0;
	char* pMsg = http_generator_tostring(httpMsg, &nLen);
	
	OnResponse(pMsg, nLen);

	free(pMsg);
	http_generator_destroy(httpMsg);
	httpMsg = NULL;
}


void CRTConnHttp::OnHttpMessage(http_message* httpMsg)
{
    
	const char* pPath = httpMsg->request_path;
	//const char* pQuery = httpMsg->query_string;
	const char* pContent = httpMsg->body;
	int nContentLen = (int)httpMsg->body_size;
    if (!pPath || !pContent) {
        LE("OnHttpMessage pPath or pContent error\n");
		//SendResponse(HPS_NOT_ACCEPTABLE, "");
        return;
    }
    memset((void*)&m_mmsg, 0, sizeof(MEETMSG));
    std::string str(pContent, nContentLen), err;
    m_mmsg.GetMsg(str, err);
    if (err.length() > 0) {
        LE("OnHttpMessage pContent error:%s\n", err.c_str());
		//SendResponse(HPS_NOT_ACCEPTABLE, "");
        return;
    }

    if (strcmp(pPath, "/login") == 0) {
        if (m_mmsg._from.length()>0 && m_mmsg._pass.length()>0) {
            OnLogin(m_mmsg._from.c_str(), m_mmsg._pass.c_str());
        } else {
            LE("login params errors\n");
        }
    } else if(strcmp(pPath, "/sndmsg") == 0) {
        if (m_mmsg._from.length()>0) {
            const char* pContentDump = strdup(pContent);
            OnSndMsg(m_mmsg._from.c_str(), m_mmsg._mtype, pContentDump, (int)strlen(pContentDump));
            free((void*)pContentDump);
            pContentDump = NULL;
        } else {
            LE("sndmsg params errors\n");
        }
    } else if(strcmp(pPath, "/getmsg") == 0) {
        if (m_mmsg._from.length()>0) {
            OnGetMsg(m_mmsg._from.c_str(), m_mmsg._mtype);
        } else {
            LE("getmsg params errors\n");
        }
    } else if (strcmp(pPath, "/logout") == 0) {
        if (m_mmsg._from.length()>0) {
            OnLogout(m_mmsg._from.c_str());
        } else {
            LE("logout params errors\n");
        }
    } else {
        SendResponse(HPS_NOT_ACCEPTABLE, "");
        return;
    }
}
