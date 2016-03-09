#ifndef __EF_NET_LOG_H__
#define __EF_NET_LOG_H__

#include "base/ef_btype.h"
#include "base/ef_log.h"
#include <string>


namespace ef{

	void setNetLogName(const std::string& name);
	void setNetLogPath(const std::string& path);
	void setNetLogLevel(int level);


	Logger getNetLog(int level);
	//getLog()(getNetLogName(), level)

        #define NLogDebug getNetLog(LOG_LEVEL_DEBUG) << "[PID=" \
			<< getpid() << "] [THREAD=" \
			<< pthread_self() <<  "] [" << __FILE__ << ":" \
			<< __LINE__ << "] [" << __FUNCTION__ << "] "
        #define NLogTrace getNetLog(LOG_LEVEL_TRACE) << "[PID=" \
			<< getpid() << "] [THREAD=" \
			<< pthread_self() <<  "] ["<< __FILE__ << ":" \
			<< __LINE__ << "] [" << __FUNCTION__ << "] "
        #define NLogInfo getNetLog(LOG_LEVEL_INFO) << "[PID=" \
			<< getpid() << "] [THREAD=" \
			<< pthread_self() <<  "] [" << __FILE__ << ":" \
			<< __LINE__ << "] [" << __FUNCTION__ << "] "
        #define NLogWarn getNetLog(LOG_LEVEL_WARN) << "[PID=" \
			<< getpid() << "] [THREAD=" \
			<< pthread_self() <<  "] [" << __FILE__ << ":" \
			<< __LINE__ << "] [" << __FUNCTION__ << "] "
        #define NLogError getNetLog(LOG_LEVEL_ERROR) << "[PID=" \
			<< getpid() << "] [THREAD=" \
			<< pthread_self() <<  "] [" << __FILE__ << ":" \
			<< __LINE__ << "] [" << __FUNCTION__ << "] "
 
}

#endif/*EF_NET_LOG_H*/
