#include "base/ef_aes.h"
#include "base/ef_base64.h"
#include "time.h"
#include <sstream>
#include <fstream>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "ef_crypt.h"

namespace ef{

using namespace std;

static const string rawkey = "7ddcef8769474e099310ec8e71d181e1";
static int token_timeout = 7200;//2hours
static int text_timeout = 30;//30secs

int setTokenTimeout(int timeout)
{
	if (timeout > 0) {
		token_timeout = timeout;
	}
	return 0;
}

int setTextTimeout(int timeout)
{
	if (timeout > 0) {
		text_timeout = timeout;
	}
	return 0;
}

static int split(const std::string& str, std::vector<std::string>& ret_, 
		std::string sep)
{
	    if (str.empty())
	    {
		return 0;
	    }

	    std::string tmp;
	    std::string::size_type pos_begin = str.find_first_not_of(sep);
	    std::string::size_type comma_pos = 0;

	    while (pos_begin != std::string::npos)
	    {
		comma_pos = str.find(sep, pos_begin);
		if (comma_pos != std::string::npos)
		{
		    tmp = str.substr(pos_begin, comma_pos - pos_begin);
		    pos_begin = comma_pos + sep.length();
		}
		else
		{
		    tmp = str.substr(pos_begin);
		    pos_begin = comma_pos;
		}

		if (!tmp.empty())
		{
		    ret_.push_back(tmp);
		    tmp.clear();
		}
	    }
	    return 0;

}

int checkToken(const string &token, map<string, string> &minfo)
{
	string decstr = base64Decode(token);
	vector<string> vstr;
	split(decstr, vstr, "&");
	if (vstr.size() != 2) {
		return AUTHERR_DECFAIL;
	}
	if (abs(time(NULL) - atoi(vstr[0].data())) > token_timeout) {
		return AUTHERR_TIMEOUT;
	}

	string rawbody;
	if (aesDecrypt(base64Decode(vstr[1]), rawkey + vstr[0], rawbody) < 0) {
		return AUTHERR_DECFAIL;
	}
	
	vstr.clear();
	split(rawbody, vstr, "&");
	vector<string>::iterator it = vstr.begin();
	for (; it != vstr.end(); it++) {
		string::size_type pos = it->find("="); 
		minfo[it->substr(0, pos)] = it->substr(pos + 1);
	}
	return AUTHERR_OK;
}

int generateToken(const map<string, string> &minfo, string &token)
{
	stringstream ss;
	
	ss << time(NULL);
	string tm = ss.str();
	string key = rawkey + tm;
	ss.str("");
	map<string, string>::const_iterator it = minfo.begin();
	for (; it != minfo.end(); it++)  {
		ss << it->first << "=" << it->second << "&";
	}
	string enctext;
	
	if (aesEncrypt(ss.str(), key, enctext) < 0) {
		return -1;
	}
	token = base64Encode(tm + "&" + base64Encode(enctext));
	return AUTHERR_OK;  
}

int encrypt(const string &text, string &enctext)
{
	stringstream ss;
	time_t tm = time(NULL);
	ss << tm;
	srand(tm);
	string encbody;
	if (aesEncrypt(text, ss.str(), encbody) < 0) {
        return AUTHERR_ENCFAIL;
    }
    ss << "&" << rand() << "&" << base64Encode(encbody);
    enctext = base64Encode(ss.str());
	return AUTHERR_OK;
}

int decrypt(const string &enctext, string &text)
{
	string decstr = base64Decode(enctext);
    vector<string> vstr;
    split(decstr, vstr, "&");
    if (vstr.size() != 3) {
	return AUTHERR_DECFAIL;
    }
    if (abs(time(NULL) - atoi(vstr[0].data())) > text_timeout) {
        return AUTHERR_TIMEOUT;
    }
   
    if (aesDecrypt(base64Decode(vstr[2]), vstr[0], text) < 0) {
        return AUTHERR_DECFAIL;
    }

	return AUTHERR_OK;	

}
};
