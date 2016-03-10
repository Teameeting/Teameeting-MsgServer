#include "data_watcher.h"
#include "zk_client.h"

namespace gim{

int DataWatcherBase::init(ZKClient* c, const std::string& path){
	setPath(path);	
	addToClient(c);

	int ret = c->getNodeData(path, m_data);

	if(ret < 0){
		return ret;
	}

	ret = addWatch(ZKWatcher::CHANGE_EVENT);

	return ret;
}

int JsonWatcherBase::init(ZKClient* c, const std::string& path){
	int ret = DataWatcherBase::init(c, path);
	
	if(ret < 0){
		return ret;
	}
	
	Json::Reader r;	

	if(!r.parse(getData(), m_v)){
		return -1;
	}

	return 0;
}

int JsonWatcherBase::onDataChange(int ver, const std::string& s){
	DataWatcherBase::onDataChange(ver, s);

	Json::Reader r;

	if(r.parse(s, m_v)){
		return onJsonChange(ver, m_v);
	}

	return 0;
}


}
