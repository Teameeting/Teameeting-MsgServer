#include "http_generator.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int http_msg_len_with_content(http_message *msg);

http_message *http_generator_create(enum http_type type)
{
	http_message *msg =  (http_message*)malloc(sizeof(struct http_message));
	if(msg)
	{
		memset(msg, 0, sizeof(struct http_message));
		msg->type = type;
		msg->method = HTTP_POST;
		msg->status_code = HPS_OK;
		msg->http_major = 1;
		msg->http_minor = 1;
	}
	return msg;
}

void http_generator_set_method(http_message *msg, enum http_method method)
{
	if(msg)
		msg->method = method;
}

void http_generator_set_status(http_message *msg, enum http_status status)
{
	if(msg)
		msg->status_code = status;
}

int http_generator_set_path(http_message *msg, const char* path, int pathLen)
{
	//int pathLen = 0;
	if(!msg || !path || pathLen <= 0)
		return -1;
	//pathLen = strlen(path);
	if(pathLen > MAX_HEADER_SIZE)
		return -2;
	memcpy(msg->request_path, path, pathLen);
	return 0;
}

int http_generator_set_query(http_message *msg, const char* query, int queryLen)
{
	//int queryLen = 0;
	if(!msg || !query || queryLen <= 0)
		return -1;
	//queryLen = strlen(query);
	if(queryLen > MAX_HEADER_SIZE)
		return -2;
	memcpy(msg->query_string, query, queryLen);
	return 0;
}

int http_generator_set_fragment(http_message *msg, const char* fragment, int fragmentLen)
{
	//int fragmentLen = 0;
	if(!msg || !fragment || fragmentLen <= 0)
		return -1;
	//fragmentLen = strlen(fragment);
	if(fragmentLen > MAX_HEADER_SIZE)
		return -2;
	memcpy(msg->fragment, fragment, fragmentLen);
	return 0;
}

int http_generator_add_header(http_message *msg, enum http_header hdr, const char* value, int valueLen)
{
	const char* pHeader = NULL;
	if(!msg || !value || valueLen <= 0)
		return -1;
	pHeader = http_header_str(hdr);
	if((strlen(pHeader) + valueLen + 2) > MAX_HEADER_SIZE)
		return -2;
	msg->headers[msg->num_headers][0] = (char *) malloc(strlen(pHeader) + 1);
	strcpy(msg->headers[msg->num_headers][0], pHeader);
	msg->headers[msg->num_headers][1] = (char *) malloc(valueLen + 1);
	strcpy(msg->headers[msg->num_headers][1], value);
	//sprintf(msg->headers[msg->num_headers], "%s: %s", pHeader, value);
	msg->num_headers ++;
	return 0;
}

int http_generator_set_content(http_message *msg, const char* content, int contentLen, const char* type)
{
	char buffer[64] = {0};
	if(!msg || !content || contentLen <= 0)
		return -1;
	msg->body_size = contentLen;
	msg->body = (char *)malloc(contentLen + 1);
	memcpy(msg->body, content, contentLen);
	msg->body[contentLen] = '\0';
	if(type)
	{
		http_generator_add_header(msg, HPR_CONTENT_TYPE, type, strlen(type));
		sprintf(buffer, "%d", contentLen);
		http_generator_add_header(msg, HPR_CONTENT_LENGTH, buffer, strlen(buffer));
	}
	return 0;
}

char*http_generator_tostring(http_message *msg, int *nLen)
{
	char* pMsg = NULL;
	int msgLen = 0;
	int msgPos = 0;
	int i = 0;
	if(!msg)
		return 0;
	
	msgLen = http_msg_len_with_content(msg);
	if(msgLen <= 0)
		return NULL;
	msgLen += 1;
	if(msg->type == HTTP_REQUEST)
	{// Generate http request
		pMsg = (char*)malloc(msgLen);
		{// Generate http request line.
			msgPos = sprintf(pMsg + msgPos, "%s /%s", http_method_str(msg->method), msg->request_path);
			if(strlen(msg->query_string) > 0)
			{
				msgPos += sprintf(pMsg + msgPos, "?%s", msg->query_string);
				if(strlen(msg->fragment) > 0)
					msgPos += sprintf(pMsg + msgPos, "#%s", msg->fragment);
			}

			msgPos += sprintf(pMsg + msgPos, " HTTP/%d.%d\r\n", msg->http_major, msg->http_minor);
		}
	}
	else if(msg->type == HTTP_RESPONSE)
	{// Generate http response
		pMsg = (char*)malloc(msgLen);
		{// Generate http response line
			msgPos = sprintf(pMsg + msgPos, "HTTP/%d.%d %d %s\r\n", msg->http_major, msg->http_minor, msg->status_code, http_status_str(msg->status_code));
		}
		
	}
	{// Generate http headers
		// Headers's length
		for(i = 0; i < msg->num_headers; i++)
		{
			msgPos += sprintf(pMsg + msgPos, "%s: %s\r\n", msg->headers[i][0], msg->headers[i][1]);
			//msgPos += sprintf(pMsg + msgPos, "%s\r\n", msg->headers[i]);
		}
		msgPos += sprintf(pMsg + msgPos, "\r\n");
	}
	if(msg->body_size > 0)
	{
		//msgPos += sprintf(pMsg + msgPos, "%s", msg->body);
		memcpy(pMsg + msgPos, msg->body, msg->body_size);
		msgPos += msg->body_size;
	}
	*nLen = msgLen-1;
	return pMsg;
}

void http_generator_destroy(http_message *msg)
{
	int i = 0;
	if(msg)
	{
		for(i = 0; i < MAX_HEADERS; i++)
		{
			free(msg->headers[i][0]);
			free(msg->headers[i][1]);
		}
		free(msg->body);
		free(msg->upgrade);
		free(msg);
		msg = NULL;
	}
}

// Internal fuction
static int http_msg_len_with_content(http_message *msg)
{
	int msgLen = 0;
	int pathLen = 0;
	int queryLen = 0;
	int fragmentLen = 0;
	int i = 0;
	if(msg->type == HTTP_REQUEST)
	{/*GET /ALogin?account=xxx#hello HTTP/1.1\r\n*/
		{
			// Method's length
			msgLen += strlen(http_method_str(msg->method));
			// Space
			msgLen++;
			// Path + query + fragment
			pathLen = strlen(msg->request_path);
			queryLen = strlen(msg->query_string);
			fragmentLen = strlen(msg->fragment);
			if(pathLen > 0)
			{// 
				msgLen += pathLen + 1;
				if(queryLen > 0)
				{
					msgLen += queryLen + 1;
					if(fragmentLen >0)
						msgLen += fragmentLen +1;
				}
			}
			// Space
			msgLen++;
			// Protocol
			msgLen += 8;
			// \r\n
			msgLen += 2;
		}
	}
	else if(msg->type == HTTP_RESPONSE)
	{/*HTTP/1.1 200 OK\r\n*/
		{
			// Protocol
			msgLen += 8;
			// Space
			msgLen++;
			// Method's length
			msgLen += getIntDigit(msg->status_code);
			msgLen ++;	// Space
			msgLen += strlen(http_status_str(msg->status_code));
			// \r\n
			msgLen += 2;
		}
	}
	else
		return 0;
	/*Host: localhost:8080\r\n*/
	{
		// Headers's length
		for(i = 0; i < msg->num_headers; i++)
		{
			// Header
			msgLen += (strlen(msg->headers[i][0]) + 1 + 1 + strlen(msg->headers[i][1]));
			//msgLen += strlen(msg->headers[i]);
			// \r\n
			msgLen += 2;
		}
		// \r\n
		msgLen += 2;
	}
	/*Content*/
	msgLen += msg->body_size;
	return msgLen;
}