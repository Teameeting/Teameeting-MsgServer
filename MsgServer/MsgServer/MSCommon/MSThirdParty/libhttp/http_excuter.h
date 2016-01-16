/* Copyright Avcon, Inc. All rights reserved.
 *
 * File:http_excuter.h
 *
 * Auth: Maozonwu
 * Create: 2014-6-27
 * Modify: 2014-6-27
 */
#ifndef http_excuter_h
#define http_excuter_h

#include "http_common.h"
#include "http_generator.h"
#include "http_parser.h"
HP_CPP(extern "C" {)

typedef void (*HTTP_PARSER_MSG_CB)(const void*pUserData,http_message* httpMsg);

RTK_HTTP_API int do_http_parse(enum http_type type, char* pBuf, int nLen, enum http_errno* err, HTTP_PARSER_MSG_CB cb, void* userdata);

RTK_HTTP_API int do_http_digest_auth(http_message* httpMsg, const char* pChallenge);

HP_CPP(})
#endif 	// http_excuter_h