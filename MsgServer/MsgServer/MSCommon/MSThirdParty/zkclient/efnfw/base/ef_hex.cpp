#include "ef_hex.h"


namespace ef{

	inline char  hex_char(int32 i){
		if(i >= 0 && i <= 9){
			return '0' + i;
		}else if(i >= 10 && i <= 15){
			return 'a' + i - 10;
		}

		return 0;
	}

	char* byteToHex(uint8 c, char buf[2]){

		buf[0] = hex_char(c / 16);
		buf[1] = hex_char(c % 16);

		return buf;
	}

	inline int char_to_i(char c){
		switch(c){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return c - '0';
		case 'A':
		case 'a':
			return 10;
		case 'B':
		case 'b':
			return 11;
		case 'C':
		case 'c':
			return 12;
		case 'D':
		case 'd':
			return 13;
		case 'E':
		case 'e':
			return 14;
		case 'F':
		case 'f':
			return 15;
		}
		return 0;
	}

	int hexToByte(const char buf[2]){
		return (char_to_i(buf[0]) << 4) + char_to_i(buf[1]);	
	}

	int32 bytesToHexs(const char* bytes, int32 len, char* out, int32 outlen){
		int i = 0;
		for(; i < len && i < outlen / 2; i++){
			byteToHex(*(uint8*)(bytes + i), out + i * 2);
		}
		return i * 2;	
	}

	int32 bytesToHexs(const std::string& bytes, std::string& hex){
		hex.resize(bytes.size() * 2);	
		return bytesToHexs(bytes.data(), bytes.size(), 
			const_cast<char*>(hex.data()), hex.size());
	}	

	int32 hexToBytes(const char* hex, int32 len, char* out, int32 outlen){
		int i = 0;
		for(; i < len / 2; ++i){
			out[i] = hexToByte(hex + 2 * i);
		}
		return i;
	}

	int32 hexToBytes(const std::string& hex, std::string& bytes){
		bytes.resize(hex.size() / 2);
		return hexToBytes(hex.data(), hex.size(), 
			const_cast<char*>(bytes.data()), hex.size());	
	}	

};
