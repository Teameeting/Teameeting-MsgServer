#include "http_excuter.h"
#include <stdlib.h>
#include <string.h>

struct HttpExcuterData
{
	void*				mUserData;
	HTTP_PARSER_MSG_CB	mCb;
	http_message*		mHttpMsg;
	
};

static int http_message_begin(http_parser *parser)
{
	struct HttpExcuterData* exuterData = (struct HttpExcuterData*)parser->data;
	if(exuterData)
	{
		exuterData->mHttpMsg = http_generator_create(parser->type);
		exuterData->mHttpMsg ->last_header_element = FIELD;
		exuterData->mHttpMsg ->method = parser->method;
	}
	return 0;
}

static int http_request_url(http_parser* parser, const char *data, size_t length)
{
	struct HttpExcuterData* exuterData = (struct HttpExcuterData*)parser->data;
	http_message *httpMsg = exuterData->mHttpMsg;
	if(httpMsg)
	{
		const char* pptr = data;
		const char* pfd = strstr(pptr, "?");
		if(pfd)
		{
			http_generator_set_path(httpMsg, pptr, pfd - pptr);
			pptr = pfd + 1;
			pfd = strstr(pptr, "#");
			if(pfd)
			{
				http_generator_set_query(httpMsg, pptr, pfd - pptr);
				pptr = pfd + 1;
				pfd = data + length;
				http_generator_set_fragment(httpMsg, pptr, pfd - pptr);
			}
			else
			{
				pfd = data + length;
				http_generator_set_query(httpMsg, pptr, pfd - pptr);
			}
		}
		else
		{
			http_generator_set_path(httpMsg, data, length);
		}

	}
	return 0;
}
static int http_response_status(http_parser* parser, const char *data, size_t length)
{
	struct HttpExcuterData* exuterData = (struct HttpExcuterData*)parser->data;
	http_message *httpMsg = exuterData->mHttpMsg;
	if(httpMsg)
	{
		httpMsg->status_code = parser->status_code;
	}
	return 0;
}

static int http_header_field(http_parser* parser, const char *data, size_t length)
{
	struct HttpExcuterData* exuterData = (struct HttpExcuterData*)parser->data;
	http_message *httpMsg = exuterData->mHttpMsg;
	if(httpMsg)
	{
		if(httpMsg->headers[httpMsg->num_headers][0] == NULL)
		{
			httpMsg->headers[httpMsg->num_headers][0] = (char *) malloc(length + 1);
			memcpy(httpMsg->headers[httpMsg->num_headers][0], data, length);
			httpMsg->headers[httpMsg->num_headers][0][length] = '\0';
		}

		if(httpMsg->last_header_element == VALUE)
		{
			httpMsg->last_header_element = FIELD;
		}
	}
	return 0;
}

static int http_header_value(http_parser* parser, const char *data, size_t length)
{
	struct HttpExcuterData* exuterData = (struct HttpExcuterData*)parser->data;
	http_message *httpMsg = exuterData->mHttpMsg;
	if(httpMsg)
	{
		if(httpMsg->headers[httpMsg->num_headers][1] == NULL)
		{
			httpMsg->headers[httpMsg->num_headers][1] = (char *) malloc(length + 1);
			memcpy(httpMsg->headers[httpMsg->num_headers][1], data, length);
			httpMsg->headers[httpMsg->num_headers][1][length] = '\0';
		}

		if(httpMsg->last_header_element == FIELD)
		{
			httpMsg->last_header_element = VALUE;
			httpMsg->num_headers++;
		}
	}
	return 0;
}


static int http_headers_complete(http_parser *parser)
{
	const char* pValue = NULL;
	struct HttpExcuterData* exuterData = (struct HttpExcuterData*)parser->data;
	http_message *httpMsg = exuterData->mHttpMsg;
	if(httpMsg)
	{
		httpMsg->last_header_element = FIELD;
		pValue = http_msg_header_value(HPR_TRANSFER_ENCODING, httpMsg);
		if((parser->content_length == ULLONG_MAX) && ((pValue == NULL) || (strcmp(pValue, "chunked") != 0)))
		{
			parser->content_length = 0;
		}
	}
	return 0;
}

static int http_body(http_parser* parser, const char *data, size_t length)
{
	struct HttpExcuterData* exuterData = (struct HttpExcuterData*)parser->data;
	http_message *httpMsg = exuterData->mHttpMsg;
	if(httpMsg)
	{
		http_generator_set_content(httpMsg, data, length, NULL);
	}
	return 0;
}

static int http_message_complete(http_parser *parser)
{
	struct HttpExcuterData* exuterData = (struct HttpExcuterData*)parser->data;
	http_message *httpMsg = exuterData->mHttpMsg;
	parser->content_length = 0;
	if(exuterData->mCb)
	{
		exuterData->mCb(exuterData->mUserData, exuterData->mHttpMsg);
	}
	if(httpMsg)
	{
		http_generator_destroy(httpMsg);
	}
	exuterData->mHttpMsg= NULL;
	return 0;
}

static http_parser_settings parser_settings =
  {http_message_begin
  ,http_request_url
  ,http_response_status
  ,http_header_field
  ,http_header_value
  ,http_headers_complete
  ,http_body
  ,http_message_complete
  };

//======================================================================================
int do_http_parse(enum http_type type, char* pBuf, int nLen, enum http_errno* err, HTTP_PARSER_MSG_CB cb, void* userdata)
//http_message *do_http_parse(enum http_type type, char* pBuf, int nLen, int* parsed, enum http_errno* err, enum ProcessResult* pr)
{
	int parsed = 0;
	http_parser parser;
	struct HttpExcuterData exuterData;
	if(!pBuf || nLen == 0)
		return 0;

	memset(&exuterData, 0, sizeof(struct HttpExcuterData));
	http_parser_init(&parser, type);
	exuterData.mCb = cb;
	exuterData.mUserData = userdata;
	parser.data = &exuterData;
	parsed = http_parser_execute(&parser, &parser_settings, pBuf, nLen);
	*err = HTTP_PARSER_ERRNO(&parser);

	if(exuterData.mHttpMsg)
	{
		http_generator_destroy(exuterData.mHttpMsg);
		exuterData.mHttpMsg = NULL;
	}
	return parsed;
}

int do_http_digest_auth(http_message* httpMsg, const char* pChallenge)
{
	return 0;
}