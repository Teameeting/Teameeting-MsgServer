#ifndef __X_JS_BUFFER_H__
#define __X_JS_BUFFER_H__

class XJSBuffer
{
public:
	XJSBuffer(void);
	virtual ~XJSBuffer(void);

	void RecvData(const char*message, int nLen);
	void ParseMessage(const char*message, int nLen);

	virtual void OnRecvMessage(const char*message, int nLen) = 0;

public:
	static unsigned short readShort(char** pptr);
	static void writeShort(char** pptr, unsigned short anInt);

private:
	char			*m_pBuffer;
	int				m_nBufLen;
	int				m_nBufOffset;
	char			*m_pParseBuf;
	int				m_nParseBufLen;
};

#endif	// __X_JS_BUFFER_H__