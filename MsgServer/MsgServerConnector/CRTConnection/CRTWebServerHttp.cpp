#include "CRTWebServerHttp.h"
#include "http_excuter.h"
#include "md5digest.h"
#include "rtklog.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

#include <stdlib.h>


long long CRTWebServerHttp::m_test_conn = 0;

void CRTWebServerHttp::OnParsedHttpMessage(const void*pUserData, http_message* httpMsg)
{
	CRTWebServerHttp* pHttp = (CRTWebServerHttp*)pUserData;
	if(pHttp)
	{
		pHttp->OnHttpMessage(httpMsg);
	}
}

CRTWebServerHttp::CRTWebServerHttp(void)
: m_bAuthed(false)
, m_nTimer(10)
, m_nHttpMethod(HTTP_POST)
{
}

CRTWebServerHttp::~CRTWebServerHttp(void)
{
}

int CRTWebServerHttp::ProcessData(const char*pData, int nLen)
{
	int parsed = 0;
	enum http_errno err;
	while (parsed < nLen)
	{
		const char* pMsg = pData + parsed;
		int msgLen = nLen - parsed;
		int nlen = do_http_parse(HTTP_REQUEST, (char *)pMsg, msgLen, &err, &CRTWebServerHttp::OnParsedHttpMessage, this);
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

char* CRTWebServerHttp::GenerateResponse(int code, const std::string&params, const char*pData, int nDataLen, int&outLen)
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

void CRTWebServerHttp::SendResponse(int code, const std::string&params, const char*pData, int nDataLen)
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

void CRTWebServerHttp::SendResponse(int code, const std::string&strContent)
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


void CRTWebServerHttp::OnHttpMessage(http_message* httpMsg)
{
	const char* pPath = httpMsg->request_path;
	//const char* pQuery = httpMsg->query_string;
	const char* pContent = httpMsg->body;
	int nContentLen = (int)httpMsg->body_size;
    if (!pPath || !pContent) {
        LE("OnHttpMessage pPath or pContent error\n");
		SendResponse(HPS_NOT_ACCEPTABLE, "path content err");
        return;
    }

    LI("CRTWebServerHttp::OnHttpMessage path:%s, pContent:%s\n", pPath, pContent);
    if (strcmp(pPath, "/otherlogin") == 0) {
        OnOtherLogin(pContent);
        SendResponse(HPS_OK, "200");
    } else {
        SendResponse(HPS_NOT_ACCEPTABLE, "not acceptable");
        return;
    }
}
