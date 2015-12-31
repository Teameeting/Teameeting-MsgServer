#include "RTConnHttp.h"
#include "http_excuter.h"
#include "md5digest.h"
#include "rtklog.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"

long long RTConnHttp::m_test_conn = 0;
extern int ConsumeUntil(const char*& inStr, std::string &outStr, char token);
/*
static int ParseKV(const char*str, int len, char token, XParams&params)
{
	std::string strKey;
	std::string strValue;
	const char* pStr = str;
	int pos = 0;
	while(1)
	{
		// End of params?
		if (pos >= len)
		  break;

		pos += ConsumeUntil(pStr, strKey, '=');

		if(pStr[0] == '=')
		{
			pos ++; pStr ++;
			pos += ConsumeUntil(pStr, strValue, token);
			if(pStr[0] == token)
			{
				pos ++; pStr ++;
			}
			params[strKey]=strValue;
			strKey.clear();
			strValue.clear();
		}
		else
		{
			return 0;
		}
	}

	return 1;
}
*/

void RTConnHttp::OnParsedHttpMessage(const void*pUserData, http_message* httpMsg)
{
	RTConnHttp* pHttp = (RTConnHttp*)pUserData;
	if(pHttp)
	{
		pHttp->OnHttpMessage(httpMsg);
	}
}

RTConnHttp::RTConnHttp(void)
: m_bAuthed(false)
, m_nTimer(10)
, m_nHttpMethod(HTTP_POST)
{
}

RTConnHttp::~RTConnHttp(void)
{
}

int RTConnHttp::ProcessData(const char*pData, int nLen)
{
	int parsed = 0;
	enum http_errno err;
	while (parsed < nLen)
	{
		const char* pMsg = pData + parsed;
		int msgLen = nLen - parsed;
        //LI("RTConnHttp::ProcessData do_http_parse call RTConnHttp::OnParsedHttpMessage\n");
		int nlen = do_http_parse(HTTP_REQUEST, (char *)pMsg, msgLen, &err, &RTConnHttp::OnParsedHttpMessage, this);
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

char* RTConnHttp::GenerateResponse(int code, const std::string&params, const char*pData, int nDataLen, int&outLen)
{
	http_message *httpMsg = http_generator_create(HTTP_RESPONSE);
	httpMsg->status_code = (http_status)code;

	// ÔÊÐí¿çÓò·ÃÎÊ
	http_generator_add_header(httpMsg, HPR_ACCESS_CONTROL_ALLOW_ORIGIN, "*", 1);
	
	if(params.length() > 0)
	{
		http_generator_add_header(httpMsg, HPR_PRAGMA, params.c_str(), params.length());
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

void RTConnHttp::SendResponse(int code, const std::string&params, const char*pData, int nDataLen)
{
	http_message *httpMsg = http_generator_create(HTTP_RESPONSE);
	httpMsg->status_code = (http_status)code;

	// ÔÊÐí¿çÓò·ÃÎÊ
	http_generator_add_header(httpMsg, HPR_ACCESS_CONTROL_ALLOW_ORIGIN, "*", 1);
	
	if(params.length() > 0)
	{
		http_generator_add_header(httpMsg, HPR_PRAGMA, params.c_str(), params.length());
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

void RTConnHttp::SendResponse(int code, const std::string&strContent)
{
	http_message *httpMsg = http_generator_create(HTTP_RESPONSE);
	httpMsg->status_code = (http_status)code;

	// ÔÊÐí¿çÓò·ÃÎÊ
	http_generator_add_header(httpMsg, HPR_ACCESS_CONTROL_ALLOW_ORIGIN, "*", 1);
	
	if(strContent.length() > 0)
	{
		http_generator_set_content(httpMsg, strContent.c_str(), strContent.length(), "text/json");
	}

	int nLen = 0;
	char* pMsg = http_generator_tostring(httpMsg, &nLen);
	
	OnResponse(pMsg, nLen);

	free(pMsg);
	http_generator_destroy(httpMsg);
	httpMsg = NULL;
}


void RTConnHttp::OnHttpMessage(http_message* httpMsg)
{
    
	const char* pPath = httpMsg->request_path;
	const char* pQuery = httpMsg->query_string;
	const char* pContent = httpMsg->body;
	int nContentLen = httpMsg->body_size;
    LI("RTConnHttp::OnHttpMessage conn number:%d\n", RTConnHttp::m_test_conn++);
	if(pPath && strlen(pPath) > 1)
	{
		if(strcmp(pPath, "/login") == 0)
		{
            if(NULL == pContent || nContentLen == 0)
		    {
			    SendResponse(HPS_NOT_ACCEPTABLE, "");
			    return;
		    }
            rapidjson::Document		jsonReqDoc;
		    if (jsonReqDoc.ParseInsitu<0>((char*)pContent).HasParseError())
		    {
                LI("ParseInsite hasParseError\n");
                SendResponse(HPS_BAD_REQUEST, "");
                return;
            }
            if(!(jsonReqDoc.HasMember("from") && jsonReqDoc["from"].IsString()))
            {
                SendResponse(HPS_NOT_ACCEPTABLE, "");
                return;
            }
            const char* from = jsonReqDoc["from"].GetString();
            if(!(jsonReqDoc.HasMember("pass") && jsonReqDoc["pass"].IsString()))
            {
                SendResponse(HPS_NOT_ACCEPTABLE, "");
                return;
            }
            const char* pass = jsonReqDoc["pass"].GetString();
            if (from && pass) {
                OnLogin(from, pass);
            } else {
                LI("login params errors\n");
            }

        } else if(strcmp(pPath, "/sndmsg") == 0)
        {
            if(NULL == pContent || nContentLen == 0)
		    {
			    SendResponse(HPS_NOT_ACCEPTABLE, "");
			    return;
		    }
            rapidjson::Document		jsonReqDoc;
		    if (jsonReqDoc.ParseInsitu<0>((char*)pContent).HasParseError())
		    {
                LI("ParseInsite hasParseError\n");
                SendResponse(HPS_BAD_REQUEST, "");
                return;
            }
            
            if(!(jsonReqDoc.HasMember("mtype") && jsonReqDoc["mtype"].IsInt()))
            {
                SendResponse(HPS_NOT_ACCEPTABLE, "");
                return;
            }
            int mtype = jsonReqDoc["mtype"].GetInt();
            
            if (mtype) {
                OnSndMsg(mtype, pContent, nContentLen);
            } else {
                LI("sndmsg params errors\n");
            }
		} else if(strcmp(pPath, "/getmsg") == 0)
        {
            if(NULL == pContent || nContentLen == 0)
		    {
			    SendResponse(HPS_NOT_ACCEPTABLE, "");
			    return;
		    }
            rapidjson::Document		jsonReqDoc;
		    if (jsonReqDoc.ParseInsitu<0>((char*)pContent).HasParseError())
		    {
                LI("ParseInsite hasParseError\n");
                SendResponse(HPS_BAD_REQUEST, "");
                return;
            }
            if(!(jsonReqDoc.HasMember("from") && jsonReqDoc["from"].IsString()))
            {
                SendResponse(HPS_NOT_ACCEPTABLE, "");
                return;
            }
            const char* from = jsonReqDoc["from"].GetString();
            if (from) {
                OnGetMsg(from);
            } else {
                LI("getmsg params errors\n");
            }
        } else if (strcmp(pPath, "/logout") == 0)
        {
            if(NULL == pContent || nContentLen == 0)
		    {
			    SendResponse(HPS_NOT_ACCEPTABLE, "");
			    return;
		    }
            rapidjson::Document		jsonReqDoc;
		    if (jsonReqDoc.ParseInsitu<0>((char*)pContent).HasParseError())
		    {
                LI("ParseInsite hasParseError\n");
                SendResponse(HPS_BAD_REQUEST, "");
                return;
            }
            if(!(jsonReqDoc.HasMember("from") && jsonReqDoc["from"].IsString()))
            {
                SendResponse(HPS_NOT_ACCEPTABLE, "");
                return;
            }
            const char* from = jsonReqDoc["from"].GetString();
            if (from) {
                OnLogout(from);
            } else {
                LI("logout params errors\n");
            }

        } else {
			SendResponse(HPS_NOT_ACCEPTABLE, "");
			return;
        }
		
	}
}
