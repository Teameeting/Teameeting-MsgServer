/* Copyright Avcon, Inc. All rights reserved.
 *
 * File:http_generator.h
 *
 * Auth: Maozonwu
 * Create: 2014-6-27
 * Modify: 2014-6-28
 */
#ifndef http_generator_h
#define http_generator_h

#include "http_common.h"
HP_CPP(extern "C" {)

RTK_HTTP_API http_message *http_generator_create(enum http_type type);

RTK_HTTP_API void	http_generator_set_method(http_message *msg, enum http_method method);

RTK_HTTP_API void	http_generator_set_status(http_message *msg, enum http_status status);

RTK_HTTP_API int	http_generator_set_path(http_message *msg, const char* path, int pathLen);

RTK_HTTP_API int	http_generator_set_query(http_message *msg, const char* query, int queryLen);

RTK_HTTP_API int	http_generator_set_fragment(http_message *msg, const char* fragment, int fragmentLen);

RTK_HTTP_API int	http_generator_add_header(http_message *msg, enum http_header hdr, const char* value, int valueLen);

RTK_HTTP_API int	http_generator_set_content(http_message *msg, const char* content, int contentLen, const char* type);

RTK_HTTP_API char*	http_generator_tostring(http_message *msg, int *nLen);

RTK_HTTP_API void	http_generator_destroy(http_message *msg);

HP_CPP(})
#endif 	// http_generator_h