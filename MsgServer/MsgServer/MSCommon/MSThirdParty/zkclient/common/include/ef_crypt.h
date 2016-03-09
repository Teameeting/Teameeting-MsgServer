#ifndef __MYENCRYPT_H__
#define __MYENCRYPT_H__

#include <string>
#include <vector>
#include <map>

namespace ef {

#define AUTHERR_OK 0
#define AUTHERR_TIMEOUT -1
#define AUTHERR_DECFAIL -2
#define AUTHERR_ENCFAIL -3

int setTokenTimeout(int timeout);

int setTextTimeout(int timeout);

int generateToken(const std::map<std::string, std::string> &minfo, std::string &token);

int checkToken(const std::string &token, std::map<std::string, std::string> &minfo);

int encrypt(const std::string &text, std::string &enctext);

int decrypt(const std::string &enctext, std::string &text);

};
#endif
