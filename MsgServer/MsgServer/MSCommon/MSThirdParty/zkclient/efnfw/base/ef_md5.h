/*
 * Copyright (c) 2004-2005 Sergey Lyubka <valenok@gmail.com>
 * All rights reserved
 *
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Sergey Lyubka wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 */
 /****************************************************************
 *
 * Thanks for Sergey Lynbka, I hope I could meet U and buy U a beer 
 *
 * ***************************************************************/

#ifndef __EF_MD5_H__
#define	__EF_MD5_H__

#include "ef_btype.h"
#include <string>

namespace ef{

typedef struct MD5Context {
	uint32	buf[4];
	uint32	bits[2];
	uint8	in[64];
} MD5_CTX;

void MD5Init(MD5_CTX *ctx);
void MD5Update(MD5_CTX *ctx, uint8 const *buf, uint32 len);
void MD5Final(uint8 digest[16], MD5_CTX *ctx);
void MD5(uint8 digest[16], uint8 const *buf, uint32 len);
void MD5Hex(std::string& hex, uint8 const *buf, uint32 len);

int32 fileMD5 (char const * fname, uint8 * md5val);

}

#endif /*MD5_HEADER_INCLUDED */
