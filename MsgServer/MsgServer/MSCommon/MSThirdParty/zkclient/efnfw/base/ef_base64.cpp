#include "ef_base64.h"
#include <string.h>

namespace ef{

static const char*  ch64="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string base64Encode(const std::string& s){
    int n = s.size(); 
    int buflen = n;
    int i;
    const unsigned char *buf;
    unsigned char *dst;
    std::string ret; 

    if(n % 3 != 0)  /* pad with '=' by using a temp buffer */
    {
        buflen = n + 3 - n % 3;
    }

    ret.resize(buflen * 4 / 3);

    buf = (const unsigned char*)s.data();
    dst = (unsigned char*)ret.data();

    for(i = 0; i <= n - 3; i += 3, dst += 4){
        dst[0] = ch64[ (buf[i] & 0xFC) >> 2 ];
        dst[1] = ch64[ ((buf[i] & 0x03) << 4) + ((buf[i + 1] & 0xF0) >> 4) ];
        dst[2] = ch64[ ((buf[i + 1] & 0x0F) << 2) + ((buf[i + 2] & 0xC0) >> 6) ];
        dst[3] = ch64[ buf[i + 2] & 0x3F ]; 
    }
    
    if(n % 3){
        i = n - n % 3;
        *dst++ = ch64[buf[i] >> 2];
        if(n % 3 > 1){
            *dst++ = ch64[((buf[i] & 0x03) << 4) + (buf[i + 1] >> 4)];
            *dst++ = ch64[(buf[i + 1] & 0x0f) << 2];
            *dst++ = '=';
        }else{
            *dst++ = ch64[((buf[i] & 0x03) << 4)];
            *dst++ = '=';
            *dst++ = '=';
        }         
    }
    return ret;
}

static const short b64_reverse_table[256] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

std::string base64Decode(const std::string& s){
    std::string ret;
    int l,n,i,j;
    const unsigned char *p;
    unsigned char *dst; 
    int pandcnt = 0;
 
    l = n = s.size();
    if(n % 4){
        n = n - n % 4;
    }
    ret.resize(n / 4 * 3);
    p = (const unsigned char*)s.data();
    dst = (unsigned char*)ret.data();

    int a,b,c,d;

    for(i = 0, j = 0; i <= l - 4; i += 4, j += 3)
    {
       a = b64_reverse_table[p[i]];
       b = b64_reverse_table[p[i + 1]];
       c = p[i + 2] != '=' ? b64_reverse_table[p[i + 2]] : (pandcnt = 2, 0);
       if(pandcnt == 0){
           d = p[i + 3] != '=' ? b64_reverse_table[p[i + 3]] : (pandcnt = 1, 0);
       }else{
           d = 0;
       }
       if(a < 0 || b < 0 || c < 0 || d < 0){
           ret.resize(j);
           return ret;
       }
       dst[j]=(a << 2) + ((b & 0x30) >> 4);
       dst[j+1]=((b & 0x0F) << 4) + ((c & 0x3C) >> 2);
       dst[j+2]=((c & 0x03) << 6) + d;
    }
    ret.resize(j - pandcnt);
    return ret;
}

}
