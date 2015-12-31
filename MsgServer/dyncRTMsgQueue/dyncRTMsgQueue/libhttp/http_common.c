#include "http_common.h"
#include <assert.h>
#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* Array */ 
static const char *method_strings[] =
  {
#define XX(num, name, string) #string,
  HTTP_METHOD_MAP(XX)
#undef XX
  };

/* Array  */
static const int status_index[] =
{
#define XX(num, name, string) num,
  HTTP_STATUS_MAP(XX)
#undef XX
};
static const char *status_strings[] =
  {
#define XX(num, name, string) #string,
  HTTP_STATUS_MAP(XX)
#undef XX
  };

/* Array */
static const char *header_strings[] =
  {
#define XX(name, string) #string,
  HTTP_HEADER_MAP(XX)
#undef XX
  };

//=================================================================
//
int 
getIntDigit(int n)
{
	return (n/1>0)?(1+getIntDigit(n/10)):0;
}

const char *
http_method_str (enum http_method m)
{
  return ELEM_AT(method_strings, m, "<unknown>");
}

int http_status_index(enum http_status hps)
{
	int size = ARRAY_SIZE(status_index);
	int i = 0;
	for(i = 0; i < size; i++)
	{
		if(status_index[i] == hps)
			return i;
	}
	return 0;
}

const char *
http_status_str(enum http_status hps)
{
	return ELEM_AT(status_strings, http_status_index(hps), "Error");
}

const char *
http_header_str(enum http_header hdr) 
{
	return ELEM_AT(header_strings, hdr, "Unkown");
}

const char *
http_msg_header_value(enum http_header hdr, http_message *msg)
{
	int i = 0;
	const char* pValue = NULL;
	const char* pHeader = http_header_str(hdr);
	if(msg)
	{
		for(; i < msg->num_headers; i++)
		{
			if(strcmp(pHeader, msg->headers[i][0]) == 0)
			{
				pValue = msg->headers[i][1];
				break;
			}
		}
	}
	return pValue;
}
