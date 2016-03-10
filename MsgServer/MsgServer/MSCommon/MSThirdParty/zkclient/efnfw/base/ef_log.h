#ifndef	__EF_LOG_H__
#define	__EF_LOG_H__

#include <map>
#include <time.h>
#include <string>
#include <iostream>
#include <sstream>
#include "ef_btype.h"
#include "ef_thread.h"

//this is thread safe
namespace ef{


	enum{
		LOG_LEVEL_ALL = 0,// level string "ALL"
		LOG_LEVEL_DEBUG = 1, // level string "DEBUG"
		LOG_LEVEL_TRACE = 2, // level string "TRACE"
		LOG_LEVEL_INFO = 3, // level string "INFO"
		LOG_LEVEL_WARN = 4, // level string "WARN"
		LOG_LEVEL_ERROR = 5, // level string "ERROR"
		LOG_LEVEL_NOT = 6, //level string "NOT"
	};

	enum{
		LOG_TYPE_NORMAL = 0,
		LOG_TYPE_BINLOG = 1,
	};

	
	const char* getLevelStr(int32 level);
	int32 getStrLevel(const char* level);

	std::string getStrDay(time_t n);
	std::string getStrHour(time_t n);
	std::string getStrMinute(time_t n); 
	std::string getStrTimeAndPid(time_t n);
	time_t getMinuteTimestamp(time_t n);
	time_t getHourTimestamp(time_t n);
	time_t getDayTimestamp(time_t n);

	class Appender{
	public:
		Appender():m_refcnt(1){
		}

		virtual ~Appender(){
		}

		virtual int32 write_s(const std::string& s){
			return 0;
		}

		virtual int32 take();
		//if refcnt < 1 ,delete   
		virtual int32 give();

	protected:
		volatile int32 m_refcnt;

	private:
		Appender(const Appender&);
		Appender& operator = (const Appender&);
	};

	class ConsoleAppender:public Appender{
	public:
		virtual int32 write_s(const std::string& s){
			std::cout << s;
			return 0;
		}

	};

	class nullAppender:public Appender{
	public:

		virtual int32 write_s(const std::string& s){
			return 0;
		}

	};

	class FileAppender:public Appender{
	public:
		enum{
			MINUTE = 60,
			HOUR = 3600,
			DAY = 3600 * 24,
		};
		enum{
			NOCOMPRESS = 0,
			GZIP = 1,
			DEFAULT_CACHE_SIZE = 8192,
		};

		FileAppender(const std::string& path, int32 schedu_span, 
				bool immediately_flush = false, 
				int32 max_cache_size = DEFAULT_CACHE_SIZE);
		virtual ~FileAppender();
		virtual int32 take();
		virtual int32 write_s(const std::string& s);
	private:
		bool lock();
		bool unlock();
		int32 flush_cache(std::string* s);
		MUTEX m_cs;
		std::string m_path;
		std::string m_caches[2];
		int32 m_cache_idx;
		int32 m_schedu_span;
		time_t m_last_flush_time;
		time_t m_last_open_time; 
		FILE* m_file;
		bool m_immediately_flush;
		int32 m_max_cache_size;
		volatile int32 m_flushlock;
	};


	class Logger{
	public:
		Logger(const std::string& name = "", 
				int32 type = LOG_TYPE_NORMAL, 
				int32 level = LOG_LEVEL_ALL,
				Appender* p = NULL)
			:m_name(name), m_type(type), 
			m_level(level), m_append(p){
				if(p){
					p->take();
				}
			}

		const Logger& swap(const Logger& l) const{
			Appender* ap = m_append;
			m_name = l.m_name;
			m_type = l.m_type;
			m_level = l.m_level;
			m_append = l.m_append;
			l.m_append = NULL;
			if(ap){
				ap->give();
			}
			return *this;
		}

		~Logger(){
			if(m_append){
				writeToAppender();
			}
		}

		Logger(const Logger& l):
			m_type(l.m_type),
			m_append(l.m_append)
		{
			l.m_append = NULL;
		}

		int32 setHeader() const;

		const Logger& operator = (const Logger& l) const{
			return swap(l);
		}


		template<class T>
			const Logger& operator << (const T& t) const{
				if(m_append)
					m_os << t;
				return *this;
			}

	private:
		int32 writeToAppender();

		mutable std::string m_name;
		mutable int32 m_type;
		mutable int32 m_level;
		mutable Appender* m_append;
		mutable std::stringstream m_os;
	};


	class LoggerC{
	public:
		LoggerC():m_level(LOG_LEVEL_NOT), m_Appender(NULL){};
		~LoggerC(){};

		void setName(const std::string& name){
			m_name = name;
		}

		void setAppender(Appender* a){
			m_Appender = a;
		}

		void setLevel(int32 level){
			m_level = level;	
		}

		Logger getLogger(int32 level);
		Logger getLogger(const std::string& name, int32 level);
	private:
		std::string m_name;
		int32 m_level;
		Appender* m_Appender;
	};

	class log{
	public:
		log();
		~log();
		Logger operator()(const std::string& name, int32 level);
		int32 addAppender(const std::string& name, 
				const std::string& path = "",
				int32 scheduspan = FileAppender::DAY,
				bool imdflush = false);
		int32 addLogger(const std::string& name, 
				int32 level, const std::string& Appender);
		int32 setDefaultLogger(int32 level, const std::string& Appender);
	private:

		typedef std::map<std::string, LoggerC> Loggers;
		typedef std::map<std::string, Appender*> Appenders;


		//MUTEX m_cs;
		Loggers m_logs;
		Appenders m_appends;
		LoggerC m_default_Logger;
	};

	class SimpleLog{
	public:
		SimpleLog():m_appender(NULL){};
		~SimpleLog(){delete m_appender;};
		
		void setPath(const std::string& p);
		void setName(const std::string& n);
		void setLevel(int level);		

		Logger operator()(int32 level);

	private:
		LoggerC m_log;
		Appender* m_appender;
	};


	log& getLog(); 
	Logger logDebug(const std::string& logname);
	Logger logTrace(const std::string& logname);
	Logger logInfo(const std::string& logname);
	Logger logWarn(const std::string& logname);
	Logger logError(const std::string& logname);

}

#endif
