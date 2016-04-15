#ifndef __EF_AES_H__
#define __EF_AES_H__

#include <string>
#include "ef_btype.h"


namespace ef{

	/*  Error Codes  */
#define     AES_BAD_KEY_DIR          -1 /*  Key direction is invalid, e.g., unknown value */
#define     AES_BAD_KEY_MAT          -2 /*  Key material not of correct length */
#define     AES_BAD_KEY_INSTANCE     -3 /*  Key passed is not valid */
#define     AES_BAD_CIPHER_MODE      -4 /*  Params struct passed to cipherInit invalid */
#define     AES_BAD_CIPHER_STATE     -5 /*  Cipher in wrong state (e.g., not initialized) */
#define     AES_BAD_BLOCK_LENGTH     -6
#define     AES_BAD_CIPHER_INSTANCE  -7
#define     AES_BAD_DATA             -8 /*  Data contents are invalid, e.g., invalid padding */
#define     AES_BAD_OTHER            -9 /*  Unknown error */

	int32 aesEncrypt(const std::string& src, 
		const std::string& key, std::string& dst);
	int32 aesEncrypt(const char* src, size_t len, 
		const std::string& key, std::string& dst);
	int32 aesDecrypt(const std::string& src, 
		const std::string& key, std::string& dst);
	int32 aesDecrypt(const char* src, size_t len,
		const std::string& key, std::string& dst);
};

#endif
