#ifndef	__LOG_INIT_H__
#define	__LOG_INIT_H__

#include <string>
#include "base/ef_log.h"

namespace gim{
	#define TYPE_FILE_APPENDER	"FileAppender"
	#define TYPE_CONSOLE_APPENDER	"ConsoleAppender"
	#define TYPE_NULL_APPENDER	"NullAppender"

	int logInit(const std::string& config); 
}

#endif
