#ifndef __RT_CONN_HTTP_H__
#define __RT_CONN_HTTP_H__
#include "http_common.h"
#include <map>
#include <string>

// Keyvalue: key=value
typedef std::map<std::string, std::string> XParams;

class RTConnHttp
{
public:
	RTConnHttp(void);
	virtual ~RTConnHttp(void);

	int ProcessData(const char*pData, int nLen);

	char* GenerateResponse(int code, const std::string&query, const char*pData, int nLen, int&outLen);
	void SendResponse(int code, const std::string&query, const char*pData, int nLen);
	void SendResponse(int code, const std::string&strContent);

	static void OnParsedHttpMessage(const void*pUserData, http_message* httpMsg);
	void OnHttpMessage(http_message* httpMsg);

public:
    virtual void OnLogin(const char* pUserid, const char* pPass) = 0;
    virtual void OnSndMsg(int mType, const char* pData, int dLen) = 0;
    virtual void OnGetMsg(const char* pUserid) = 0;
    virtual void OnLogout(const char* pUserid) = 0;
	virtual void OnResponse(const char*pData, int nLen)=0;
protected:
    static long long m_test_conn;
private:
	bool		m_bAuthed;
	int			m_nTimer;
	http_method	m_nHttpMethod;
};

#endif	// __RT_CONN_HTTP_H__