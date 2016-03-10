#ifndef ZK_CLIENT_H
#define ZK_CLIENT_H


#include <map>
#include <set>
#include <string>
#include <vector>
#include <pthread.h>
#include <zookeeper/zookeeper.h>

namespace gim{

typedef std::vector<std::string> s_vector;
typedef struct _zhandle zhandle_t;

class ZKWatcher;
class ServerNode;

std::string toZkEventType(int type);
std::string toZkState(int state);
std::string zkPath(const std::string& parent, const std::string& child);
typedef void (*ZKLogCallBack)(void* ctx, const std::string& l);

class ZKClient{
public:
	enum{
		STATUS_OK = 0,
		STATUS_NOT_INIT = -10,
	};

	enum{
		DEFAULT_TIMEOUT = 30000,
	};

	ZKClient();
	~ZKClient();

	int init(const std::string& zkurl, 
		int timout_ms = DEFAULT_TIMEOUT);

	int clear();

	int createEphemeralNode(const std::string& path, 
		const std::string& data);
	int createPersistentNode(const std::string& path, 
		const std::string& data);
	int deleteNode(const std::string& path);
	int setNodeData(const std::string& path, 
		const std::string& data);
	//no watch
	int getNodeData(const std::string& path, std::string& data);
	//no watch
	int getChildren(const std::string& path, s_vector& us);
	void setLogFn(void* ctx, ZKLogCallBack fn){
		m_logfn = fn;
		m_logctx = ctx;
	}
	void writeLog(const std::string& l){
		if(m_logfn)
			m_logfn(m_logctx, l);
	}
	void addSrvNode(ServerNode* node){
		m_srvnodes.insert(node);
	}
	void delSrvNode(ServerNode* node){
		m_srvnodes.erase(node);
	}
	void srvNodeRebuild();
private:

	struct WatchCtx{
		WatchCtx(ZKClient* c, int i)
			:cli(c), watcher_id(i){}
		ZKClient* cli;
		int watcher_id;
	};

	class CtxAutoDeletor{
	public:
		CtxAutoDeletor(WatchCtx* ctx):m_ctx(ctx){
		}

		~CtxAutoDeletor(){
			if(m_ctx)
				delete m_ctx;
		}
		
	private:
		WatchCtx* m_ctx;
	};

	int doInit();
	int reinit();
	void watchAll();
	int connectingFinishSignal();
	int waitConnectingFinish();

	static void watcherFnG(zhandle_t * zh, int type, int state,
		const char *path, void *watcherCtx);

	static void watcherFn(zhandle_t * zh, int type, int state,
		const char *path, void *watcherCtx);

	static void getChildrenComplete(int rc, const String_vector* s_v, 
		const void* data);
	static void getDataComplete(int rc, const char *value, int value_len, 
		const struct Stat *stat, const void *data);
	static void getExistsComplete(int rc, const struct Stat *stat, 
		const void *data);

	int addWatcher(ZKWatcher* w);
	int delWatcher(ZKWatcher* w);
	ZKWatcher* getWatcher(int id);

	int watchData(ZKWatcher* w);
	int watchChildren(ZKWatcher* w);
	int watchExists(ZKWatcher* w);
	int watchCreate(ZKWatcher* w);
#if 0
	int clearPathWatchers();
	int addChildrenWatcher(const std::string& path);
	int getChildren(const std::string& path, urls& us);
	bool getChildrenFromCache(const std::string& path, urls& us);
	bool addChildrenCacheIfNoExist(const std::string& path, 
		const urls&us, urls& oldus);
	int broadcastChildrenChange(const std::string& path, const urls& us); 

#endif	

	friend class ZKWatcher;


	typedef std::map<int, ZKWatcher*> watcherset;
	std::string m_zkurl;
	int m_timeout_ms;
	zhandle_t* m_zkhandle;
	pthread_mutex_t m_cs;
	pthread_cond_t m_cond;
	watcherset m_watchers;
	bool m_waiting;

	ZKLogCallBack m_logfn;
	void* m_logctx;

	std::set<ServerNode*> m_srvnodes;
};


};


#endif
