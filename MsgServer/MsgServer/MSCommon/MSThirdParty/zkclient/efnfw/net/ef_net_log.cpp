#include "ef_net_log.h"
#include "ef_common.h"

namespace ef{
	static SimpleLog g_log;


	void setNetLogName(const std::string& name){
		g_log.setName(name);
	}

	void setNetLogPath(const std::string& path){
		g_log.setPath(path);

	}

	void setNetLogLevel(int level){
		g_log.setLevel(level);
	}

	Logger getNetLog(int level){
		return g_log(level);
	}


}

