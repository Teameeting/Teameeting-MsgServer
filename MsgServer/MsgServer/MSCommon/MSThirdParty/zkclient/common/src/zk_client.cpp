#include "zk_client.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include "base/ef_thread.h"
#include "zk_watcher.h"
#include "server_status.h"

namespace gim{

	using namespace std;
	using namespace ef;

	typedef s_vector urls;

	std::string toZkEventType(int type){
		if(ZOO_CHILD_EVENT == type)
			return "ZOO_CHILD_EVENT";

		if(ZOO_CHANGED_EVENT == type)
			return "ZOO_CHANGED_EVENT";

		if(ZOO_CREATED_EVENT == type)
			return "ZOO_CREATED_EVENT";

		if(ZOO_DELETED_EVENT == type)
			return "ZOO_DELETED_EVENT";

		if(ZOO_SESSION_EVENT == type)
			return "ZOO_SESSION_EVENT";

		stringstream os;
		os << type;
		return os.str();
	}

	std::string toZkState(int state){
		if(ZOO_EXPIRED_SESSION_STATE == state)
			return "ZOO_EXPIRED_SESSION_STATE";

		if(ZOO_AUTH_FAILED_STATE == state)
			return "ZOO_AUTH_FAILED_STATE";

		if(ZOO_CONNECTING_STATE == state)
			return "ZOO_CONNECTING_STATE";

		if(ZOO_ASSOCIATING_STATE == state)
			return "ZOO_ASSOCIATING_STATE";

		if(ZOO_EXPIRED_SESSION_STATE == state)
			return "ZOO_EXPIRED_SESSION_STATE";

		if(ZOO_CONNECTED_STATE == state)
			return "ZOO_CONNECTED_STATE";

		stringstream os;
		os << state;
		return os.str();
	}
	std::string zkPath(const std::string& parent, const std::string& child){
		std::string path = parent + "/" + child;
		std::string old = "//";
		while(1){
			string::size_type   pos(0);
			if((pos=path.find(old)) != std::string::npos)
				path.replace(pos,old.size(),"/");
			else
				break;
		}
		return path;
	}
	void ZKClient::watcherFnG(zhandle_t * zh, int type, int state,
		const char *path, void *watcherCtx){
		ZKClient* c = (ZKClient*)watcherCtx;

		if (c->m_logfn){
			stringstream ss;
			ss << "ZKClient::watcherFnG(), path:" << path << ", type:" << toZkEventType(type)
				<< ", state:" << toZkState(state) << ",zkclient=" << c;
			c->writeLog(ss.str());
		}

		if(type == ZOO_SESSION_EVENT){
			if(state == ZOO_CONNECTED_STATE){
				c->connectingFinishSignal();
			}
			else if(state == ZOO_EXPIRED_SESSION_STATE){
				//should reinit
				if(0 == c->reinit()){
					c->srvNodeRebuild();
					c->watchAll();
				}
			}
			return;
		}
	}
	void ZKClient::watcherFn(zhandle_t * zh, int type, int state,
		const char *path, void *watcherCtx){

        if(type == ZOO_SESSION_EVENT) {
			return;
        }

		WatchCtx* ctx = (WatchCtx*)watcherCtx;
		CtxAutoDeletor d(ctx);
		ZKClient* c = (ZKClient*)ctx->cli;
		ZKWatcher* w = (ZKWatcher*)c->getWatcher(ctx->watcher_id);

		if (c->m_logfn){
			stringstream ss;
			ss << "ZKClient::watcherFn(), path:" << path << ", type:" << toZkEventType(type)
				<< ", state:" << toZkState(state) << ",zkclient=" << c;
			c->writeLog(ss.str());
		}

		//watcher has been deleted
		if(!w){
			return;
		}

		if(type == ZOO_CHILD_EVENT){
			w->handleEvent(ZKWatcher::CHILD_EVENT);
		}else if(type == ZOO_CHANGED_EVENT){
			w->handleEvent(ZKWatcher::CHANGE_EVENT);
		}else if(type == ZOO_DELETED_EVENT){
			w->handleEvent(ZKWatcher::DELETE_EVENT);
			w->onPathRemove(path);
		}else if(type == ZOO_CREATED_EVENT){
			w->handleEvent(ZKWatcher::CREATE_EVENT);
		}
	}
	void ZKClient::getChildrenComplete(int rc, const String_vector* s_v, const void* data){

		WatchCtx* ctx = (WatchCtx*)data;
		CtxAutoDeletor d(ctx);
		ZKClient* p = (ZKClient*)ctx->cli;

		ZKWatcher* w = (ZKWatcher*)p->getWatcher(ctx->watcher_id);
		if (p->m_logfn){
			stringstream ss;
			if (w){
				ss << ", path:" << w->getPath();
			}
			else{
				ss << ", can not find watcher!";
			}
			p->writeLog(ss.str());
		}


		if(!w){
			return;
		}

		if(rc == ZNONODE){
			w->checkExists();
			return;

		}

		if(ZOK != rc){
			return;
		}

		urls us;
		for(int i = 0; i < s_v->count; ++i){
			us.push_back(s_v->data[i]);
		}

		w->onChildrenChange(us);
	}


	void ZKClient::getDataComplete(int rc, const char *value, int value_len,
		const struct Stat *stat, const void *data){

		WatchCtx* ctx = (WatchCtx*)data;
		CtxAutoDeletor d(ctx);
		ZKClient* p = (ZKClient*)ctx->cli;

		ZKWatcher* w = (ZKWatcher*)p->getWatcher(ctx->watcher_id);
		if (p->m_logfn){
			stringstream ss;
			if (w){
				ss << ", path:" << w->getPath();
			}
			else{
				ss << ", can not find watcher!";
			}
			p->writeLog(ss.str());
		}

		if(!w){
			return;
		}

		if(rc == ZNONODE){
			w->checkExists();
			return;
		}

		if(ZOK != rc){
			return;
		}

		w->onDataChange(stat->version, string(value, value_len));
	}


	void ZKClient::getExistsComplete(int rc, const struct Stat *stat, const void *data){

		if(!data){
			return;
		}

		WatchCtx* ctx = (WatchCtx*)data;
		CtxAutoDeletor d(ctx);
		ZKClient* p = (ZKClient*)ctx->cli;

		ZKWatcher* w = (ZKWatcher*)p->getWatcher(ctx->watcher_id);
		if (p->m_logfn){
			stringstream ss;
			ss << "ZKClient::getChildrenComplete() rc:" << rc << ", id:" << ctx->watcher_id;
			if (w){
				ss << ", path:" << w->getPath();
			}
			else{
				ss << ", can not find watcher!";
			}
			p->writeLog(ss.str());
		}


		if(!w){
			return;
		}

		if(rc == ZOK){
			w->onPathCreate();
		}else if(rc == ZNONODE){
			w->onPathRemove();
		}
	}


	ZKClient::ZKClient()
		:m_timeout_ms(DEFAULT_TIMEOUT),
		m_zkhandle(NULL), m_waiting(false), m_logfn(NULL){
			pthread_mutex_init(&m_cs, NULL);
			pthread_cond_init(&m_cond, NULL);
	}

	ZKClient::~ZKClient(){
		clear();
	}


	int ZKClient::createEphemeralNode(const string& path,
		const string& data){
		int ret = 0;
		ret = zoo_create(m_zkhandle, path.data(), data.data(),
			data.size(), &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, NULL, 0);
		if(ret){
			if (m_logfn){
				stringstream ss;
				ss << "ZKClient::createEphemeralNode() path:" << path << " fail, error:" << zerror(ret);
				writeLog(ss.str());
			}
			return -1;
		}
		return ret;
	}

	int ZKClient::createPersistentNode(const string& path,
		const string& data){
		int ret = 0;
		ret = zoo_create(m_zkhandle, path.data(), data.data(),
			data.size(), &ZOO_OPEN_ACL_UNSAFE, 0, NULL, 0);
		if(!ret){
			return 0;
		}

		if (ret && m_logfn){
			stringstream ss;
			ss << "ZKClient::createPersistentNode() path:" << path << " fail, error:" << zerror(ret);
			writeLog(ss.str());
		}
		return ret;
	}


	int ZKClient::deleteNode(const string& path){
		int ret = 0;
		ret = zoo_delete(m_zkhandle, path.data(), -1);
		if (ret && m_logfn){
			stringstream ss;
			ss << "ZKClient::deleteNode() path:" << path << " fail, error:" << zerror(ret);
			writeLog(ss.str());
		}

		return ret;
	}

	int ZKClient::setNodeData(const string& path, const string& data){
		int ret = 0;
		ret = zoo_set(m_zkhandle, path.data(), data.data(), data.size(), -1);

		if (ret && m_logfn){
			stringstream ss;
			ss << "ZKClient::setNodeData() path:" << path << " fail, error:" << zerror(ret);
			writeLog(ss.str());
		}

		return ret;
	}


	int ZKClient::getNodeData(const string& path, string& data){
		int ret = 0;
		struct Stat s;
		data.resize(1024 * 1000);
		char* buf = (char*)data.data();
		int buflen = data.size();
		ret = zoo_get(m_zkhandle, path.data(), 0, buf, &buflen, &s);
		if(!ret){
			data.resize(buflen);
			return 0;
		}

		if (ret && m_logfn){
			stringstream ss;
			ss << "ZKClient::getNodeData() path:" << path << " fail, error:" << zerror(ret);
			writeLog(ss.str());
		}

		return ret;
	}

	int ZKClient::init(const string& zkurl,
		int timout_ms){
		m_zkurl = zkurl;
		m_timeout_ms = timout_ms;
		int ret = doInit();
		if(0 == ret){
			waitConnectingFinish();
			if(zoo_state(m_zkhandle)!=ZOO_CONNECTED_STATE){
				zookeeper_close(m_zkhandle);
				m_zkhandle = NULL;
				ret = -1;
			}
		}
		return ret;
	}

	int ZKClient::reinit(){
		if(m_zkhandle){
			zookeeper_close(m_zkhandle);
			m_zkhandle = NULL;
		}
		return doInit();
	}

	void ZKClient::watchAll(){
		pthread_mutex_lock(&m_cs);
		watcherset::iterator it = m_watchers.begin();
		for(; it != m_watchers.end(); ++it){
			ZKWatcher* w = it->second;
			w->nodeExists();
		}
		pthread_mutex_unlock(&m_cs);
	}
	int ZKClient::watchData(ZKWatcher* w){
		WatchCtx* ctx = new WatchCtx(this, w->getID());
		WatchCtx* ctx1 = new WatchCtx(this, w->getID());
		int ret = zoo_awget(m_zkhandle, w->getPath().data(),
			ZKClient::watcherFn, ctx, getDataComplete, ctx1);

		if (ret && m_logfn){
			stringstream ss;
			ss << "ZKClient::watchData(): path:" << w->getPath() << " id:" << w->getID() << ", ret:" << ret;
			writeLog(ss.str());
		}

		return ret;

	}

	int ZKClient::watchChildren(ZKWatcher* w){
		WatchCtx* ctx = new WatchCtx(this, w->getID());
		WatchCtx* ctx1 = new WatchCtx(this, w->getID());
		int ret = zoo_awget_children(m_zkhandle, w->getPath().data(),
			ZKClient::watcherFn, ctx, getChildrenComplete, ctx1);

		if (ret && m_logfn){
			stringstream ss;
			ss << "ZKClient::watchData(): path:" << w->getPath() << " id:" << w->getID() << ", ret:" << ret;
			writeLog(ss.str());
		}
		return ret;

	}

	int ZKClient::watchExists(ZKWatcher* w){
		WatchCtx* ctx = new WatchCtx(this, w->getID());
		WatchCtx* ctx1 = new WatchCtx(this, w->getID());
		int ret = zoo_awexists(m_zkhandle, w->getPath().data(),
			ZKClient::watcherFn, ctx,  getExistsComplete, ctx1);
		if (ret && m_logfn){
			stringstream ss;
			ss << "ZKClient::watchExists(): path:" << w->getPath() << " id:" << w->getID() << ", ret:" << ret;
			writeLog(ss.str());
		}
		return ret;

	}

	int ZKClient::watchCreate(ZKWatcher* w){
		WatchCtx* ctx = new WatchCtx(this, w->getID());
		WatchCtx* ctx1 = new WatchCtx(this, w->getID());
		int ret = zoo_awexists(m_zkhandle, w->getPath().data(),
			ZKClient::watcherFn, ctx, getExistsComplete, ctx1);

		if (ret && m_logfn){
			stringstream ss;
			ss  << "ZKClient::watchCreate: path:" << w->getPath() << " id:" << w->getID() << ", ret:" << ret;
			writeLog(ss.str());
		}

		return ret;
	}


	int ZKClient::doInit(){
		m_zkhandle = zookeeper_init(m_zkurl.data(), ZKClient::watcherFnG, m_timeout_ms, NULL, this, 0);
		return m_zkhandle == NULL?-1:0;
	}

	int ZKClient::connectingFinishSignal(){
		if(m_waiting){
			pthread_mutex_lock(&m_cs);
			pthread_cond_signal(&m_cond);
			pthread_mutex_unlock(&m_cs);
		}
		return 0;
	}

	int ZKClient::waitConnectingFinish(){
		m_waiting = true;
		pthread_mutex_lock(&m_cs);
		pthread_cond_wait(&m_cond, &m_cs);
		pthread_mutex_unlock(&m_cs);
		m_waiting = false;
		return 0;
	}

	int ZKClient::clear(){
		//clear watcher is job of who create it
		//clearPathWatchers();
		if(m_zkhandle){
			zookeeper_close(m_zkhandle);
			m_zkhandle = NULL;
		}
		pthread_mutex_destroy(&m_cs);
		pthread_cond_destroy(&m_cond);
		return 0;
	}

	int ZKClient::addWatcher(ZKWatcher* w){
		ef::AutoLock l(&m_cs);
		m_watchers[w->getID()] = w;\
		return 0;
	}

	int ZKClient::delWatcher(ZKWatcher* w){
		ef::AutoLock l(&m_cs);
		m_watchers.erase(w->getID());

		return 0;
	}


	ZKWatcher* ZKClient::getWatcher(int id){
		ef::AutoLock l(&m_cs);

		watcherset::iterator itor = m_watchers.find(id);

		if(itor == m_watchers.end()){
			return NULL;
		}

		return itor->second;
	}


	int ZKClient::getChildren(const string& path, s_vector& us){
		int ret = 0;
		struct String_vector s_v;
		ret = zoo_get_children(m_zkhandle, path.data(),
			0, &s_v);

		if (ret){
			if (m_logfn){
				stringstream ss;
				ss  << "ZKClient::getChildren: path:" << path << ", error:" << zerror(ret);
				writeLog(ss.str());
			}
			return ret;
		}

		for(int i = 0; i < s_v.count; ++i){
			us.push_back(s_v.data[i]);
		}
		deallocate_String_vector(&s_v);
		return 0;
	}
	void ZKClient::srvNodeRebuild(){
		for (std::set<ServerNode*>::iterator it = m_srvnodes.begin(); it != m_srvnodes.end(); ++it){
			ServerNode* node = *it;
			if (node){
				node->setData();
			}
		}
	}
}
