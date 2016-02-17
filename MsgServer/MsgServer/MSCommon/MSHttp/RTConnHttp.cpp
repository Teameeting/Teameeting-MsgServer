#include <stdlib.h>
#include "RTConnHttp.h"
#include "http_excuter.h"
#include "md5digest.h"
#include "rtklog.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"


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
		int nlen = do_http_parse(HTTP_RESPONSE, (char *)pMsg, msgLen, &err, &RTConnHttp::OnParsedHttpMessage, this);
		parsed += nlen;
		if (err != HPE_OK)
		{
            LE("do_http_parse error!!!!!!\n");
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

void RTConnHttp::OnHttpMessage(http_message* httpMsg)
{
    //if (httpMsg->type == HTTP_RESPONSE) {
        OnResponse(httpMsg->body, httpMsg->body_size);
    //}
}

const char* RTConnHttp::GenerateRequest(http_method method, const std::string& path, const std::string& data, int &outLen)
{
    if (path.length()==0) {
        LE("params error!!!\n");
        outLen = 0;
        return "";
    }
    
    http_message *httpMsg = http_generator_create(HTTP_REQUEST);
    
    http_generator_set_method(httpMsg, method);
    http_generator_set_path(httpMsg, path.c_str(), (int)path.length());
    http_generator_add_header(httpMsg, HPR_HOST, m_host.c_str(), (int)m_host.length());
    http_generator_add_header(httpMsg, HPR_ACCEPT, "application/json", 16);

    if(data.length()>0)
    {
        http_generator_set_content(httpMsg, data.c_str(), (int)data.length(), "application/x-www-form-urlencoded");
    }
    
    int nLen = 0;
    char* pMsg = http_generator_tostring(httpMsg, &nLen);
    outLen = nLen;
    
    http_generator_destroy(httpMsg);
    httpMsg = NULL;
    
    return pMsg;
}

const char* RTConnHttp::GenerateRequest(http_method method, const std::string& path, const std::string& data, int &outLen) const
{
    if (path.length()==0) {
        LE("params error!!!\n");
        outLen = 0;
        return "";
    }
    
    http_message *httpMsg = http_generator_create(HTTP_REQUEST);
    
    http_generator_set_method(httpMsg, method);
    http_generator_set_path(httpMsg, path.c_str(), (int)path.length());
    http_generator_add_header(httpMsg, HPR_HOST, m_host.c_str(), (int)m_host.length());
    http_generator_add_header(httpMsg, HPR_ACCEPT, "application/json", 16);
    
    if(data.length()>0)
    {
        http_generator_set_content(httpMsg, data.c_str(), (int)data.length(), "application/x-www-form-urlencoded");
    }
    
    int nLen = 0;
    char* pMsg = http_generator_tostring(httpMsg, &nLen);
    outLen = nLen;
    
    http_generator_destroy(httpMsg);
    httpMsg = NULL;
    
    return pMsg;
}
