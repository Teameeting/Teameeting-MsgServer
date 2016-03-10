#ifndef __DATA_WATCHER_H__
#define __DATA_WATCHER_H__

#include <string>
#include <vector>
#include <json/json.h>
#include "zk_watcher.h"
#include "extract_type.h"

namespace gim
{
	class ZKClient;

	class DataWatcherBase: public ZKWatcher{
	public:
		DataWatcherBase():m_version(-1){};

		virtual ~DataWatcherBase(){};

		int init(ZKClient* c, const std::string& path);

		virtual int onDataChange(int ver, const std::string& data){
			m_version = ver;
			m_data = data;
			return 0;
		}
		virtual int onPathRemove(){
			int ret = 0;
			ret = onDataChange(-1, "");
			m_version = -1;
			m_data = "";
			return ret;
		}

		const std::string& getData() const{
			return m_data;
		}

	private:
		int m_version;
		std::string m_data;

	};


	template<class T>
	class DataWatcher
		:public DataWatcherBase
	{
	public:
		//friend class ExtractType<T>::type;
        friend T;
#if defined(GPUSH_MAC)
        friend int T::CB(int, const std::string&);
#endif

		typedef int (T::*CB)(int, const std::string&);

		DataWatcher(T* t, CB c):
            DataWatcherBase(),
			m_ctx(t), m_cb(c){
		}

		virtual ~DataWatcher(){
		}

		virtual int onDataChange(int ver, const std::string& data){
            int ret = 0;
			if(getData() != data && m_ctx){
				ret = (m_ctx->*m_cb)(ver, data);
			}
			DataWatcherBase::onDataChange(ver, data);
			return ret;
		}

	private:
		T* m_ctx;
		CB m_cb;
	};


	class JsonWatcherBase: public DataWatcherBase{
	public:
		virtual ~JsonWatcherBase(){}
		int init(ZKClient* c, const std::string& path);
		virtual int onDataChange(int ver, const std::string& data);
		virtual int onJsonChange(int ver, const Json::Value& v){
			return 0;
		}
		const Json::Value& getJsonData() const{
			return m_v;
		}
	private:
		Json::Value m_v;
	};



	template<class T>
	class JsonWatcher:public JsonWatcherBase{
	public:
		typedef int (T::*CB)(int, const Json::Value&);

		JsonWatcher(T* t, CB c):m_ctx(t), m_cb(c){}
		virtual ~JsonWatcher(){}

		virtual int onJsonChange(int ver, const Json::Value& c){

			if(m_ctx)
				return (m_ctx->*m_cb)(ver, c);

			return 0;
		}

	private:
		T* m_ctx;
		CB m_cb;
	};

} //end of namespace

#endif
