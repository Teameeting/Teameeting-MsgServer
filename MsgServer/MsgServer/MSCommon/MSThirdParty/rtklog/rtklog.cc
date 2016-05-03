#include "rtklog.h"
#include "talk/base/logging.h"
#include "talk/base/fileutils.h"
#include "talk/base/pathutils.h"
#include "talk/base/stream.h"

class RtkLog
{
public:
	static RtkLog* Instance()
	{
		static RtkLog instance;
        return &instance;
	};
	void OpenStream(const char* pathName, int level)
	{
		CloseStream();

		if(NULL != pathName)
		{
			talk_base::FileStream *pFileStream = NULL;
			pFileStream = new talk_base::FileStream();
			if(pFileStream->Open(pathName, "wb", NULL))
			{
				pFileStream->DisableBuffering();
				pStream = pFileStream;
			}
			else
			{
				pFileStream->Close();
				delete pFileStream;
				pFileStream = NULL;
			}
		}

		if(!pStream)
		{
			pStream = new talk_base::NullStream();
		}

		talk_base::LogMessage::LogToStream(pStream, (enum talk_base::LoggingSeverity)level);
	}
	void CloseStream()
	{
		if(pStream)
		{
			talk_base::LogMessage::RemoveLogToStream(pStream);
			pStream->Close();
			delete pStream;
			pStream = NULL;
		}
	}
protected:
	RtkLog(void):pStream(NULL)
	{

	};
	~RtkLog(void)
	{
		CloseStream();
	};
private:
	talk_base::StreamInterface* pStream;
};

//==========================================================================
//
void L_Init(int logLevel, const char* pathName)
{
	talk_base::LogMessage::LogToDebug(logLevel);
	RtkLog::Instance()->OpenStream(pathName, logLevel);
}

void L_Printf(const char*file, int line, int level, const char *format,...)
{
	char m_pBuf[2084] = {0};
	va_list marker;
	va_start(marker,format);
	vsprintf(m_pBuf, format, marker);
	va_end(marker);

	switch(level)
	{
	case L_ALL:
		{
			LOG_RTK(file, line, LS_SENSITIVE) << m_pBuf;
			break;
		}
	case L_NORMAL:
		{
			LOG_RTK(file, line, LS_VERBOSE) << m_pBuf;
			break;
		}
	case L_INFO:
		{
			LOG_RTK(file, line, LS_INFO) << m_pBuf;
			break;
		}
	case L_WARNING:
		{
			LOG_RTK(file, line, LS_WARNING) << m_pBuf;
			break;
		}
	case L_ERROR:
		{
			LOG_RTK(file, line, LS_ERROR) << m_pBuf;
			break;
		}
	}
}

void L_Deinit()
{
	RtkLog::Instance()->CloseStream();
}
