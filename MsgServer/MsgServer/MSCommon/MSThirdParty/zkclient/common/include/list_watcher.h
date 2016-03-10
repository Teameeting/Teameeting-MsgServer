#ifndef __LIST_WATCHER_H__
#define __LIST_WATCHER_H__

#include <map>
#include <vector>
#include <json/json.h>
#include "zk_watcher.h"
#include "server_status.h"

namespace gim
{
	class ZKClient;
	typedef std::map<std::string/*id*/, std::string>  ChildrenMap;

	typedef int (*ChildrenWatcherCallBack)(void* ctx, const ChildrenMap& children);

	class ListWatcherBase
		:public ZKWatcher
	{
	public:
		ListWatcherBase():m_loaddata(true){};
		virtual ~ListWatcherBase(){};

		int init(ZKClient* c, const std::string& path, bool loaddata = true);

		int getChildren(ChildrenMap& m);

		virtual int onChildrenChange(const std::vector<std::string>& children);

		virtual int onListChange(const ChildrenMap& c){
			return 0;
		}

		virtual int onPathRemove(const std::string& path) {
			return 0;
		}

	private:
		int getChildrenData(const std::vector<std::string>& v, ChildrenMap& m);
	private:
		bool m_loaddata;
	};

	template<class T>
	class ListWatcher:public ListWatcherBase{
	public:
        friend T;
#if defined(GPUSH_MAC)
        friend int T::CB(const ChildrenMap&);
        friend int T::PCB(const std::string&);
#endif
		typedef int (T::*CB)(const ChildrenMap&);
		typedef int (T::*PCB)(const std::string&);

		ListWatcher(T* t, CB c, PCB pc):m_ctx(t), m_cb(c), m_pcb(pc){}
		virtual ~ListWatcher(){}

		virtual int onListChange(const ChildrenMap& c){

			if(m_ctx)
				return (m_ctx->*m_cb)(c);

			return 0;
		}

		virtual int onPathRemove(const std::string& path) {
			if(m_pcb)
				return (m_ctx->*m_pcb)(path);
			return 0;
		}

	private:
		T* m_ctx;
		CB m_cb;
		PCB m_pcb;
	};


	typedef std::map<std::string/*id*/, Json::Value>  JChildrenMap;
	class JListWatcherBase: public ListWatcherBase{
	public:
		virtual ~JListWatcherBase(){}
		int getChildren(JChildrenMap& m);
		virtual int onListChange(const ChildrenMap& c);
		virtual int onJsonListChange(const JChildrenMap& c){
			return 0;
		}
	private:
	};



	template<class T>
	class JListWatcher:public JListWatcherBase{
	public:
		typedef int (T::*CB)(const JChildrenMap&);

		JListWatcher(T* t, CB c):m_ctx(t), m_cb(c){}
		virtual ~JListWatcher(){}

		virtual int onJsonListChange(const JChildrenMap& c){

			if(m_ctx)
				return (m_ctx->*m_cb)(c);

			return 0;
		}

	private:
		T* m_ctx;
		CB m_cb;
	};

	class SvListWatcherBase: public ListWatcherBase{
	public:
		virtual ~SvListWatcherBase(){}
		int getChildren(SvStatusMap& m);
		virtual int onListChange(const ChildrenMap& c);
		virtual int onServerListChange(const SvStatusMap& c){
			return 0;
		}
	private:
	};



	template<class T>
	class SvListWatcher:public SvListWatcherBase{
	public:
		typedef int (T::*CB)(const SvStatusMap&);

		SvListWatcher(T* t, CB c):m_ctx(t), m_cb(c){}
		virtual ~SvListWatcher(){}

		virtual int onServerListChange(const SvStatusMap& c){

			if(m_ctx)
				return (m_ctx->*m_cb)(c);

			return 0;
		}

	private:
		T* m_ctx;
		CB m_cb;
	};


}

#endif //__CHILDREN_WATCHER_H__
