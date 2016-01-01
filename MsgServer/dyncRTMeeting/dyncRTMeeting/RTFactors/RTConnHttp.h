#ifndef __RT_CONN_HTTP_H__
#define __RT_CONN_HTTP_H__
#include "http_common.h"
#include <map>
#include <string>
#include "RTMeetMsg.h"

class RTConnHttp
{
public:
	RTConnHttp(void);
	virtual ~RTConnHttp(void);

	int ProcessData(const char*pData, int nLen);

	static void OnParsedHttpMessage(const void*pUserData, http_message* httpMsg);
	void OnHttpMessage(http_message* httpMsg);
    const char* GenerateRequest(const std::string& path, const std::string& data, int& outLen);

public:
	virtual void OnResponse(const char*pData, int nLen)=0;
protected:
    void SetHttpHost(const std::string& host) { m_host.assign(host); }
    static long long m_test_conn;
private:
	bool		m_bAuthed;
	int			m_nTimer;
	http_method	m_nHttpMethod;
    std::string m_host;
};

#endif	// __RT_CONN_HTTP_H__