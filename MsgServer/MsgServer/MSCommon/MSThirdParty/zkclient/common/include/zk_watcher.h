#ifndef _ZK_WATCHER_H_
#define _ZK_WATCHER_H_
#include <string>
#include <vector>

namespace gim
{
	class ZKClient;

	class ZKWatcher{
	public:
		enum{
			CREATE_EVENT = 1,
			DELETE_EVENT = 2,
			CHANGE_EVENT = 4,
			CHILD_EVENT = 8,
			ALL_EVENT = 0XFFFFFFFF,
		};

		ZKWatcher(const std::string& path = "");

		void setPath(const std::string& path){
			m_path = path;
		}

		//when destroy, auto stop watch
		virtual ~ZKWatcher();

		int getID() const{
			return m_id;
		}

		int addToClient(ZKClient* cli);
		int delFromClient();

		int addWatch(int eventflag);

		int delWatch(int eventflag);

		int getWatchEventMask() const{
			return m_event_mask;
		}

		int handleEvent(int event);
		int checkExists();
		int nodeExists();

		const std::string& getPath() const{
			return m_path;
		}

		virtual int onPathRemove(const std::string& path) {
			return 0;
		}

		virtual int onPathRemove(){
			return 0;
		}

		virtual int onPathCreate(){
			return 0;
		}

		virtual int onChildrenChange(const std::vector<std::string>& children){
			return 0;
		}
		
		virtual int onDataChange(int ver, const std::string& data){
			return 0;
		}

	protected:	

		int setClient(ZKClient* cli);

		ZKClient* getClient(){
			return m_cli;
		}

		ZKClient* m_cli;
		std::string m_path;
		int m_id;
		int m_event_mask;
		bool m_check_exists;
		static volatile int s_id;
	};

	typedef int (*DataWatchCallBack)(void* ctx, int version, const std::string& data);

	class ZKDataWatcher
		:public ZKWatcher
	{
	public:
		ZKDataWatcher(const std::string& path);
		virtual ~ZKDataWatcher();
		void setCb(void* ctx, DataWatchCallBack cb);
		virtual int onDataChange(const std::string& data, int version);
		virtual int onPathRemove();
		void initNodeData();
		std::string getData(){
			return m_data;
		}
	private:
		DataWatchCallBack m_cb;
		void* m_ctx;
		int m_version;
		std::string m_data;
	};
} //end of namespace

#endif
