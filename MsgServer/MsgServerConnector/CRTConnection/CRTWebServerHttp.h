#ifndef __MsgServerConnector_CRTWEBSERVERHTTP_H__
#define __MsgServerConnector_CRTWEBSERVERHTTP_H__
#include "http_common.h"
#include <map>
#include <string>


class CRTWebServerHttp
{
public:
	CRTWebServerHttp(void);
	virtual ~CRTWebServerHttp(void);

	int ProcessData(const char*pData, int nLen);

	char* GenerateResponse(int code, const std::string&query, const char*pData, int nLen, int&outLen);
	void SendResponse(int code, const std::string&query, const char*pData, int nLen);
	void SendResponse(int code, const std::string&strContent);

	static void OnParsedHttpMessage(const void*pUserData, http_message* httpMsg);
	void OnHttpMessage(http_message* httpMsg);

public:
    virtual void OnOtherLogin(const char* pContent) = 0;
	virtual void OnResponse(const char*pData, int nLen)=0;
protected:
    static long long m_test_conn;
private:
	bool		m_bAuthed;
	int			m_nTimer;
	http_method	m_nHttpMethod;
};


#endif	// __MsgServerConnector_CRTWEBSERVERHTTP_H__
