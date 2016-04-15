#include "zk_watcher.h"
#include "zk_client.h"
#include "base/ef_atomic.h"
using namespace ef;
namespace gim
{
	volatile int ZKWatcher::s_id = 0;

	ZKWatcher::ZKWatcher(const std::string& path)
		:m_cli(NULL), m_path(path), m_event_mask(0),
		m_check_exists(false){
			ef::atomicIncrement32(&s_id);
			m_id = s_id;
	}

	ZKWatcher::~ZKWatcher(){
		delFromClient();
	}

	int ZKWatcher::delFromClient(){
		if(m_cli){
			m_cli->delWatcher(this);
			m_cli = NULL;
		}

		return 0;
	}

	int ZKWatcher::setClient(ZKClient* c){
		m_cli = c;
		return 0;	
	}

	int ZKWatcher::checkExists(){

		if(!m_cli){
			return 0;
		}

		if(m_check_exists){
			return 0;
		}

		m_check_exists = true;

		return m_cli->watchCreate(this);

	}

	int ZKWatcher::addToClient(ZKClient* cli){
		cli->addWatcher(this);
		setClient(cli);

		return 0;
	}


	int ZKWatcher::addWatch(int eventflag){
		if(!m_cli){
			return -1;
		}

		if(eventflag & CHANGE_EVENT){
			m_cli->watchData(this);
		}

		if(eventflag & CHILD_EVENT){
			m_cli->watchChildren(this);
		}

		if(eventflag & CREATE_EVENT || eventflag & DELETE_EVENT){
			m_cli->watchExists(this);
		}

		m_event_mask |= eventflag;

		return 0;
	}

	int ZKWatcher::handleEvent(int event){
		std::string e = toZkEventType(event);
		if(event & m_event_mask){
			addWatch(event);
		}

		switch(event){
	case DELETE_EVENT:
		if(!(event & m_event_mask)){
			checkExists(); 
		}
		break;
	case CREATE_EVENT:
		if(!(event & m_event_mask)){
			nodeExists();
		}
		break;
		}

		return 0;
	}

	int ZKWatcher::nodeExists(){
		if(!m_cli){
			return -1;
		}

		if(m_event_mask & CHANGE_EVENT){
			m_cli->watchData(this);
		}

		if(m_event_mask & CHILD_EVENT){
			m_cli->watchChildren(this);
		}

		if(m_event_mask & CREATE_EVENT || m_event_mask & DELETE_EVENT){
			m_cli->watchExists(this);
		}

		m_check_exists = false;

		return 0;
	}

	ZKDataWatcher::ZKDataWatcher(const std::string& path)
		:ZKWatcher(path), m_cb(NULL), m_ctx(NULL), m_version(-1){

	}

	ZKDataWatcher::~ZKDataWatcher(){

	}
	void ZKDataWatcher::setCb(void* ctx, DataWatchCallBack cb){
		m_cb = cb;
		m_ctx = ctx;
	}
	int ZKDataWatcher::onDataChange(const std::string& data, int version){
		m_version = version;
		if (m_data != data){
			m_data = data;
			if (m_cb){
				return m_cb(m_ctx, version, data);
			}
		}
		
		return 0;
	}
	int ZKDataWatcher::onPathRemove(){
		return onDataChange("", -1);
	}
	void ZKDataWatcher::initNodeData(){
		m_cli->getNodeData(m_path, m_data);
	}
}
