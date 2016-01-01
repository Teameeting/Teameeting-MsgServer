/* Copyright Avcon, Inc. All rights reserved.
 *
 * File:http_common.h
 *
 * Auth: Maozonwu
 * Create: 2014-6-27
 * Modify: 2014-6-30
 */
#ifndef http_common_h
#define http_common_h
#ifdef __cplusplus
# define HP_CPP(x) x
#else
# define HP_CPP(x)
#endif
HP_CPP(extern "C" {)

#ifdef LIBHTTP_EXPORTS
#define RTK_HTTP_API _declspec(dllexport)
#elif LIBHTTP_DLL
#define RTK_HTTP_API _declspec(dllimport)
#else
#define RTK_HTTP_API
#endif

#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__) && (!defined(_MSC_VER) || _MSC_VER<1600)
#include <BaseTsd.h>
#include <stddef.h>
#include "stdint.h"
#else
#include <stdint.h>
#endif
#include <limits.h>

#ifndef MIN
# define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif	// MIN
#define MAX_HEADERS 16
#define MAX_HEADER_SIZE	512
#define MAX_ELEMENT_SIZE 1024

#ifndef ULLONG_MAX
# define ULLONG_MAX ((uint64_t) -1) /* 2^64-1 */
#endif	// ULLONG_MAX

#ifndef ARRAY_SIZE
# define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef BIT_AT
# define BIT_AT(a, i)                                                \
  (!!((unsigned int) (a)[(unsigned int) (i) >> 3] &                  \
   (1 << ((unsigned int) (i) & 7))))
#endif

#ifndef ELEM_AT
# define ELEM_AT(a, i, v) ((unsigned int) (i) < ARRAY_SIZE(a) ? (a)[(i)] : (v))
#endif

// the kinds of HTTP Methods we're interested in for
enum http_type { HTTP_REQUEST, HTTP_RESPONSE, HTTP_BOTH };

/* Request Methods 
 *
 * Map for http Request.
 */
#define HTTP_METHOD_MAP(XX)         \
  XX(0,  DELETE,      DELETE)       \
  XX(1,  GET,         GET)          \
  XX(2,  HEAD,        HEAD)         \
  XX(3,  POST,        POST)         \
  XX(4,  PUT,         PUT)          \
  /* pathological */                \
  XX(5,  CONNECT,     CONNECT)      \
  XX(6,  OPTIONS,     OPTIONS)      \
  XX(7,  TRACE,       TRACE)        \
  /* webdav */                      \
  XX(8,  COPY,        COPY)         \
  XX(9,  LOCK,        LOCK)         \
  XX(10, MKCOL,       MKCOL)        \
  XX(11, MOVE,        MOVE)         \
  XX(12, PROPFIND,    PROPFIND)     \
  XX(13, PROPPATCH,   PROPPATCH)    \
  XX(14, SEARCH,      SEARCH)       \
  XX(15, UNLOCK,      UNLOCK)       \
  /* subversion */                  \
  XX(16, REPORT,      REPORT)       \
  XX(17, MKACTIVITY,  MKACTIVITY)   \
  XX(18, CHECKOUT,    CHECKOUT)     \
  XX(19, MERGE,       MERGE)        \
  /* upnp */                        \
  XX(20, MSEARCH,     M-SEARCH)     \
  XX(21, NOTIFY,      NOTIFY)       \
  XX(22, SUBSCRIBE,   SUBSCRIBE)    \
  XX(23, UNSUBSCRIBE, UNSUBSCRIBE)  \
  /* RFC-5789 */                    \
  XX(24, PATCH,       PATCH)        \
  XX(25, PURGE,       PURGE)        \

enum http_method
  {
#define XX(num, name, string) HTTP_##name = num,
  HTTP_METHOD_MAP(XX)
#undef XX
  };

/* Map for http status constants
 *
 * 
 */
#define HTTP_STATUS_MAP(XX)													\
	XX(100, CONTINUE, Continue)											\
	XX(101, SWITHING_PROTOCOLS, Switching Protocols)						\
	XX(200, OK, OK)														\
	XX(201, CREATED, Created)												\
	XX(202, ACCEPTED, Accepted)											\
	XX(203, NON_AUTHORITATIVE_INFORMATION, Non Authoritative Information)	\
	XX(204, NO_CONTENT, No Content)										\
	XX(205, RESET_CONTENT, Reset Content)									\
	XX(206, PARTIAL_CONTENT, Partial Content)								\
	XX(300, MULTIPLE_CHOICES, Multiple Choices)							\
	XX(301, MOVED_PERMANENTLY, Moved Permanently)							\
	XX(302, FOUND, Found)													\
	XX(303, SEE_OTHER, See Other)											\
	XX(304, NOT_MODIFIED, Not Modified)									\
	XX(305, USE_PROXY, Use Proxy)											\
	XX(307, TEMPORARY_REDIRECT, Temporary Redirect)						\
	XX(400, BAD_REQUEST, Bad Request)										\
	XX(401, UNAUTHORIZED, Unauthorized)									\
	XX(402, PAYMENT_REQUIRED, Payment Required)							\
	XX(403, FORBIDDEN, Forbidden)											\
	XX(404, NOT_FOUND, Not Found)											\
	XX(405, METHOD_NOT_ALLOWED, Method Not Allowed)						\
	XX(406, NOT_ACCEPTABLE, Not Acceptable)								\
	XX(407, PROXY_AUTHENTICATION_REQUIRED, Proxy Authentication Required)	\
	XX(408, REQUEST_TIME_OUT, Request Time-out)							\
	XX(409, CONFLICT, Conflict)											\
	XX(410, GONE, Gone)													\
	XX(411, LENGTH_REQUIRED, Length Required)								\
	XX(412, PRECONDITION_FAILED, Precondition Failed)						\
	XX(413, REQUEST_ENTITY_TOO_LARGE, Request Entity Too Large)			\
	XX(414, REQUEST_URI_TOO_LARGE, Request-URI Too Large)					\
	XX(415, UNSUPPORTED_MEDIA_TYPE, Unsupported Media Type)				\
	XX(416, REQUEST_RANGE_NOT_SATISFIABLE, Request Range Not Satisfiable)	\
	XX(417, EXPECTATION_FAILED, Expectation Failed)						\
	XX(500, INTERNAL_SERVER_ERROR, Internal Server Error)					\
	XX(501, NOT_IMPLEMENTED, Not Implemented)								\
	XX(502, BAD_GATEWAY, Bad Gateway)										\
	XX(503, SERVICE_UNAVAILABLE, Service Unavailable)						\
	XX(504, GATEWAY_TIMEOUT, Gateway Timeout)								\
	XX(505, HTTP_VERSION_NOT_SUPPORTED, HTTP Version not supported)

/* Define HPS_* values for each status value above */
#define HTTP_STATUS_GEN(num, name, string) HPS_##name = num,
enum http_status {
  HTTP_STATUS_MAP(HTTP_STATUS_GEN)
};
#undef HTTP_STATUS_GEN


/* Map for http header constants
 * 
 * The provided argument should be a macro that takes 2 arguments.
 */
#define HTTP_HEADER_MAP(XX)											\
	/* VIP headers */												\
	XX(CONNECTION, Connection)/*general*/							\
    XX(DATE, Date)/*general*/										\
    XX(AUTHORIZATION, Authorization)/*request*/					\
    XX(IF_MODIFIED_SINCE, If-Modified-Since)/*request*/			\
    XX(SERVER, Server)/*response*/								\
    XX(WWW_AUTHENTICATE, WWW-Authenticate)/*response*/			\
    XX(EXPIRES, Expires)/*entity*/								\
    XX(LAST_MODIFIED, Last-Modified)/*entity*/					\
	/* Other general http headers */								\
    XX(CACHE_CONTROL, Cache-Control)								\
    XX(PRAGMA, Pragma)											\
    XX(TRAILER, Trailer)											\
    XX(TRANSFER_ENCODING, Transfer-Encoding)						\
    XX(UPGRADE, Upgrade)											\
    XX(VIA, Via)													\
    XX(WARNING, Warning)											\
	/* Other request headers */										\
    XX(ACCEPT, Accept)											\
    XX(ACCEPT_CHARSET, Accept-Charset)							\
    XX(ACCEPT_ENCODING, Accept-Encoding)							\
    XX(ACCEPT_LANGUAGE, Accept-Language)							\
    XX(EXPECT, Expect)											\
    XX(FROM, From)												\
    XX(HOST, Host)												\
    XX(IF_MATCH, If-Match)										\
    XX(IF_NONE_MATCH, If-None-Match)								\
    XX(IF_RANGE, If-Range)										\
    XX(IF_UNMODIFIED_SINCE, If-Unmodified-Since)					\
	XX(KEEP_ALIVE, Keep-Alive)										\
    XX(MAX_FORWARDS, Max-Forwards)								\
    XX(PROXY_AUTHORIZATION, Proxy-Authorization)					\
    XX(RANGE, Range)												\
    XX(REFERER, Referer)											\
    XX(TE, TE)													\
    XX(USER_AGENT, User-Agent)									\
	/* Other response headers */									\
    XX(ACCEPT_RANGES, Accept-Ranges)								\
	XX(ACCESS_CONTROL_ALLOW_ORIGIN, Access-Control-Allow-Origin)	\
    XX(AGE, Age)													\
    XX(ETAG, ETag)												\
    XX(LOCATION, Location)										\
    XX(PROXY_AUTHENTICATE, Proxy-Authenticate)					\
    XX(RETRY_AFTER, Retry-After)									\
    XX(VARY, Vary)												\
	/* Other entity headers */										\
    XX(ALLOW, Allow)												\
    XX(CONTENT_ENCODING, Content-Encoding)						\
    XX(CONTENT_LANGUAGE, Content-Language)						\
    XX(CONTENT_LENGTH, Content-Length)							\
    XX(CONTENT_LOCATION, Content-Location)						\
    XX(CONTENT_MD5, Content-MD5)									\
    XX(CONTENT_RANGE, Content-Range)								\
    XX(CONTENT_TYPE, Content-Type)								\
    /* Specific headers */											\
    XX(SEESIONCOOKIE, X-SessionCookie)							\
    XX(SERVER_IP_ADDRESS, X-Server-IP-Address)					\
	XX(ALIVE_TIME, Alive-Timer)									\
	XX(X_WINDOWSPHONE_TARGET, X-WindowsPhone-Target)				\
    XX(X_NOTIFICATIONCLASS, X-NotificationClass)									

/* Define HPR_* values for each heaher value above */
#define HTTP_HEADER_GEN(n, s) HPR_##n,
enum http_header {
  HTTP_HEADER_MAP(HTTP_HEADER_GEN)
};
#undef HTTP_HEADER_GEN

typedef struct http_message {
  enum http_type type;
  enum http_method method;
  enum http_status status_code;
  char request_path[MAX_HEADER_SIZE];
  char fragment[MAX_HEADER_SIZE];
  char query_string[MAX_ELEMENT_SIZE];
  char* body;
  size_t body_size;
  int num_headers;
  enum { NONE=0, FIELD, VALUE } last_header_element;
  char* headers [MAX_HEADERS][2];

  const char *upgrade; // upgraded body

  unsigned short http_major;
  unsigned short http_minor;
}http_message;

/* Return the digit's of a Int value. */
RTK_HTTP_API int getIntDigit(int n);

/* Returns a string version of the HTTP method. */
RTK_HTTP_API const char *http_method_str(enum http_method m);

/* Returns a string of HTTP status.*/
RTK_HTTP_API const char *http_status_str(enum http_status hps);

/* Returns a string of HTTP header.*/
RTK_HTTP_API const char *http_header_str(enum http_header hdr);

/* Return value of header in http message*/
RTK_HTTP_API const char *http_msg_header_value(enum http_header hdr, http_message *msg);

HP_CPP(})
#endif	// http_common_h