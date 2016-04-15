#ifndef __RT_HTTP_SVR_CONN_H__
#define __RT_HTTP_SVR_CONN_H__
#include "RTHttp.h"
#include "RTConnHttp.h"
#include "OSMutex.h"
#include "RTMessage.h"
#include "refcount.h"
#include "scoped_ref_ptr.h"

class RTHttpSvrConn
	: public RTHttp
	, public RTConnHttp
    , public rtc::RefCountInterface
{
public:
	RTHttpSvrConn(void);
	virtual ~RTHttpSvrConn(void);

public:


    void SendRequest(const char* pData, int nLen);

public:
	//* For RCHttp
    virtual void OnReadEvent(const char*data, int size);
    virtual int  OnWriteEvent(const char*pData, int nLen, int* nOutLen) = 0;
	virtual void OnTickEvent(){}

public:
    virtual void OnResponse(const char*pData, int nLen) {}
    virtual void OnSend(const char*pData, int nLen) {}
private:
	char			 *m_pBuffer;
	int				 m_nBufLen;
	int				 m_nBufOffset;

};

#endif	// __RT_HTTP_SVR_CONN_H__
