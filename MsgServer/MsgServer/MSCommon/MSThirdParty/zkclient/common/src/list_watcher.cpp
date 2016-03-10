#include "zk_client.h"
#include "list_watcher.h"

namespace gim
{

	int ListWatcherBase::init(ZKClient* c, const std::string& path, bool loaddata)
	{
		setPath(path);
		addToClient(c);
		addWatch(ZKWatcher::CHILD_EVENT);
		m_loaddata = loaddata;
		return 0;
	}

	int ListWatcherBase::getChildren(ChildrenMap& m){
		s_vector v;

		int ret = getClient()->getChildren(m_path, v);
		if(ret < 0)
			return ret;

		ret = getChildrenData(v, m);

		return ret; 
	}

	int ListWatcherBase::getChildrenData(const std::vector<std::string>& v, ChildrenMap& m){

		for (unsigned int n = 0; n<v.size(); n++)
		{
			std::string p = zkPath(m_path, v[n]);
			std::string data;
			int ret = getClient()->getNodeData(p, data);
			if(ret < 0){
				return ret;
			}
			m[v[n]] = data;
		}

		return 0;
	}

	int ListWatcherBase::onChildrenChange(const std::vector<std::string>& children){
		ChildrenMap m;
		
		int ret = 0;

		if(m_loaddata){
			ret = getChildrenData(children, m);
		}
		else{
			for (std::vector<std::string>::const_iterator it = children.begin();
				it != children.end(); it++){
				m[*it] = std::string();
			}
		}

		if(ret >= 0)
			return onListChange(m);
		return 0;
	}

	int JListWatcherBase::getChildren(JChildrenMap& jm){

		ChildrenMap m;
		int ret = ListWatcherBase::getChildren(m);
		
		ChildrenMap::const_iterator it = m.begin();
		for(; it != m.end(); ++it){
			Json::Value v;
			Json::Reader r;

			if(r.parse(it->second, v)){
				jm[it->first] = v;
			}	
		}

		return ret;
	}

	int JListWatcherBase::onListChange(const ChildrenMap& c){
		int ret = 0;
		JChildrenMap m;

		ChildrenMap::const_iterator it = c.begin();
		for(; it != c.end(); ++it){
			Json::Value v;
			Json::Reader r;

			if(r.parse(it->second, v)){
				m[it->first] = v;
			}	
		}

		ret = onJsonListChange(m);

		return ret;
	}

	int SvListWatcherBase::getChildren(SvStatusMap& jm){

		ChildrenMap m;
		int ret = ListWatcherBase::getChildren(m);
		
		ChildrenMap::const_iterator it = m.begin();
		for(; it != m.end(); ++it){
			ServerStatus v;
			if(v.parseFromString(it->second) >=0 ){
				jm[it->first] = v;
			}	
		}

		return ret;
	}


	int SvListWatcherBase::onListChange(const ChildrenMap& c){
		int ret = 0;
		SvStatusMap m;

		ChildrenMap::const_iterator it = c.begin();
		for(; it != c.end(); ++it){
			ServerStatus s;
			if(s.parseFromString(it->second) >= 0){
				m[it->first] = s;
			}	
		}

		ret = onServerListChange(m);

		return ret;		
	}

}//end of ef
