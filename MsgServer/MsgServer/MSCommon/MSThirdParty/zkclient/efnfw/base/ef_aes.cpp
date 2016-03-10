#include "ef_aes.h"
#include "ef_md5.h"
#include "ef_hex.h"
#include "rijndael-api-fst.h"

namespace ef{
	int32 aesEncrypt(const std::string& src, 
		const std::string& key, std::string& dst){
		return aesEncrypt(src.data(), src.size(), key, dst);
	}

	int32 aesEncrypt(const char* src, size_t len,
				const std::string& key, std::string& dst){
		int ret = 0;
		keyInstance keyist;
		u8 keymd5[16] = {0};
		u8 keymat[33] = {0};
		MD5(keymd5, (const u8*)key.data(), key.size());
		bytesToHexs((const char*)keymd5, 16, (char*)keymat, 32);
		ret = makeKey(&keyist, DIR_ENCRYPT, 128, keymat);
		if(ret < 0){
				return ret;
		}

		cipherInstance cnst;
		ret = cipherInit(&cnst, MODE_ECB, (char*)keymat);
		if(ret < 0){
				return ret;
		}

		dst.resize(len + 16 - len % 16);
		BYTE* encout = (BYTE*)dst.data();
		if(len % 16){
				ret = padEncrypt(&cnst, &keyist, (BYTE*)src, len, encout);
		}else{
				ret = blockEncrypt(&cnst, &keyist, (BYTE*)src, len, encout);
		}
		if(ret < 0)
				return ret;
		dst.resize(ret);
		dst.append((char*)&len, sizeof(int));
		return ret;
	}

	int32 aesDecrypt(const char* src, size_t len,
				const std::string& key, std::string& dst){
	int ret = 0;
	keyInstance keyist;
	u8 keymd5[16];
	u8 keymat[33] = {0};
	MD5(keymd5, (const u8*)key.data(), key.size());
	bytesToHexs((const char*)keymd5, 16, (char*)keymat, 32);
	ret = makeKey(&keyist, DIR_DECRYPT, 128, keymat);
	if(ret < 0){
			return ret;
	}

	cipherInstance cnst;
	ret = cipherInit(&cnst, MODE_ECB, (char*)keymat);
	if(ret < 0){
			return ret;
	}

	int orglen = 0;
	orglen = *(size_t*)(src + len - sizeof(orglen));
	if(orglen > (int)len - (int)sizeof(orglen)){
			return -10;
	}

	dst.resize(len + 16 - len % 16);
	BYTE* encout = (BYTE*)dst.data();
	if(orglen % 16){
			ret = padDecrypt(&cnst, &keyist, (BYTE*)src, len - sizeof(orglen), encout);
	}else{
			ret = blockDecrypt(&cnst, &keyist, (BYTE*)src, len - sizeof(orglen), encout);

	}
	if(ret < 0)
		return ret;
	dst.resize(orglen);
	return ret;
	}



int32 aesDecrypt(const std::string& src, 
		const std::string& key, std::string& dst){
	return aesDecrypt(src.data(), src.size(), key, dst);
}

	
};
