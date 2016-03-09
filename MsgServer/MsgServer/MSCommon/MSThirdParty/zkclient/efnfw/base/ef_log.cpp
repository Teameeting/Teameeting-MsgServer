#include "ef_log.h"
#include "ef_atomic.h"
#include "stdio.h"
#include "string.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <arpa/inet.h>
#endif
#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

namespace ef{

	std::string getStrDay(time_t n){
		struct tm t;
		localtime_r(&n, &t);
#ifdef USE_STRINGSTREAM
		std::stringstream os;
		os << (t.tm_year + 1900) << "_" << (t.tm_mon + 1) << "_"
			<< t.tm_mday;
		return os.str();
#else
		std::string ret;
		ret.resize(256);
		int l = snprintf((char*)ret.data(), ret.size(), "%d_%d_%d", 
			t.tm_year + 1900, 
			t.tm_mon + 1, 
			t.tm_mday);
		ret.resize(l);
		return ret;
#endif	
	}

	std::string getStrHour(time_t n){
		struct tm t;
		localtime_r(&n, &t);
#ifdef USE_STRINGSTREAM
		std::stringstream os;
		os << (t.tm_year + 1900) << "_" << (t.tm_mon + 1) << "_"
			<< t.tm_mday << "_" << t.tm_hour;
		return os.str();
#else
		std::string ret;
		ret.resize(256);
		int l = snprintf((char*)ret.data(), ret.size(), "%d_%d_%d_%d", 
			t.tm_year + 1900, 
			t.tm_mon + 1, 
			t.tm_mday,
			t.tm_hour);
		ret.resize(l);
		return ret;
#endif  
	}

	std::string getStrMinute(time_t n){
		struct tm t;
		localtime_r(&n, &t);
#ifdef USE_STRINGSTREAM
		std::stringstream os;
		os << (t.tm_year + 1900) << "_" << (t.tm_mon + 1) << "_"
			<< t.tm_mday << "_" << t.tm_hour 
			<< "_" << t.tm_min;
		return os.str();
#else
		std::string ret;
		ret.resize(256);
		int l = snprintf((char*)ret.data(), ret.size(), "%d_%d_%d_%d_%d", 
			t.tm_year + 1900, 
			t.tm_mon + 1, 
			t.tm_mday,
			t.tm_hour,
			t.tm_min);
		ret.resize(l);
		return ret;
#endif  
	}

	std::string getStrTimeAndPid(time_t n){
		struct tm t;
		localtime_r(&n, &t);
#ifdef USE_STRINGSTREAM
		std::stringstream os;
		os << (t.tm_year + 1900) << "-" << (t.tm_mon + 1) << "-"
			<< t.tm_mday << " " << t.tm_hour 
			<< ":" << t.tm_min << ":" << t.tm_sec << " ";
		return os.str();	

#else
		std::string ret;
		ret.resize(256);
		int l = snprintf((char*)ret.data(), ret.size(), "%d_%d_%d %d:%d:%d", 
			t.tm_year + 1900, 
			t.tm_mon + 1, 
			t.tm_mday,
			t.tm_hour,
			t.tm_min,
			t.tm_sec);
		ret.resize(l);
		return ret;
#endif
	}

	std::string get_str_pid(){
		std::stringstream os;
		os << getpid();
		return os.str();
	}

	time_t getMinuteTimestamp(time_t n){	
		struct tm t; 
		localtime_r(&n, &t);
		t.tm_sec = 0;
		return mktime(&t);
	}														 

	time_t getHourTimestamp(time_t n){
		struct tm t;
		localtime_r(&n, &t);
		t.tm_min = 0;
		t.tm_sec = 0;
		return mktime(&t);
	}														 

	time_t getDayTimestamp(time_t n){
		struct tm t;
		localtime_r(&n, &t);
		t.tm_hour = 0;
		t.tm_min = 0;
		t.tm_sec = 0;
		return mktime(&t);
	}

	Logger LoggerC::getLogger(int32 level){
		Logger l;
		if(level < m_level)	
			return l;

		l = Logger(m_name, LOG_TYPE_NORMAL, level, m_Appender);	
		l.setHeader();

		return l;
	}

	Logger LoggerC::getLogger(const std::string& name, int32 level){
		if(level < m_level)	
			return Logger();

		Logger l = Logger(name, LOG_TYPE_NORMAL, level, m_Appender);	
		l.setHeader();

		return l;
	}

	log::log(){
		//mutexInit(&m_cs);
	}

	log::~log(){
		Appenders::iterator itor = m_appends.begin();
		for(; itor != m_appends.end(); ++itor){
			delete itor->second;
		}
		//mutexDestroy(&m_cs);
	}

	int32 log::addAppender(const std::string& name, const std::string& path, 
				int32 span, bool flush){
		Appender* a = NULL;
		if(path.size()){
			a = new FileAppender(path, span, flush);
		}else{
			a = new ConsoleAppender();
		}
		m_appends[name] = a;
		return 0;	
	}

	int32 log::addLogger(const std::string& logname,
		int32 level, const std::string& Appender){

		Appenders::iterator itor = m_appends.find(Appender);	
		if(itor == m_appends.end())
			return -1;

		LoggerC l;
		l.setName(logname);
		l.setLevel(level);
		l.setAppender(itor->second);
		m_logs[logname] = l;

		return 0;
	}
	
	int32 log::setDefaultLogger(int32 level, const std::string& Appender){
		Appenders::iterator itor = m_appends.find(Appender);
		if(itor == m_appends.end())
			return -1;
		
		m_default_Logger.setLevel(level);
		m_default_Logger.setAppender(itor->second);

		return 0;
	}

	int32 getStrLevel(const char* level){
		int lv = 0;

		if(!level){
			return 0;
		}

		if(strncasecmp(level, "NOT", 3) == 0){
			lv = LOG_LEVEL_NOT;
		}else if(strncasecmp(level, "ALL", 3) == 0){
			lv = LOG_LEVEL_ALL;
		}else if(strncasecmp(level, "DEBUG", 5) == 0){
			lv = LOG_LEVEL_DEBUG;
		}else if(strncasecmp(level, "TRACE", 5) == 0){
			lv = LOG_LEVEL_TRACE;
		}else if(strncasecmp(level, "INFO", 4) == 0){
			lv = LOG_LEVEL_INFO;
		}else if(strncasecmp(level, "WARN", 4) == 0){
			lv = LOG_LEVEL_WARN;
		}else if(strncasecmp(level, "ERROR", 5) == 0){
			lv = LOG_LEVEL_ERROR;
		}

		return lv;
	}

	const char* getLevelStr(int32 level){
		switch(level){
			case LOG_LEVEL_ALL:
				return "[ALL] ";
			case LOG_LEVEL_DEBUG:
				return "[DEBUG] ";
			case LOG_LEVEL_TRACE:
				return "[TRACE] ";
			case LOG_LEVEL_INFO:
				return "[INFO] ";
			case LOG_LEVEL_WARN:
				return "[WARN] ";
			case LOG_LEVEL_ERROR:
				return "[ERROR] ";
			case LOG_LEVEL_NOT:
				return "[NOT] ";
		}
		return "";
	}

	int32 Logger::setHeader() const{
		if(m_os && m_type != LOG_TYPE_BINLOG){
			time_t n = time(NULL); 
			m_os << getStrTimeAndPid(n)
				<< "[" << m_name << "] "
				<< getLevelStr(m_level);
		}
		return 0;
	}

	int32 Logger::writeToAppender(){
		if(m_type == LOG_TYPE_BINLOG){
			m_append->write_s(m_os.str());
		}else{
			m_os << '\n';
			m_append->write_s(m_os.str());
		}
		m_append->give(); 
		return 0;
	}

	Logger log::operator()(const std::string& logname, int32 level){
		Logger lg;
		//mutexTake(&m_cs);
		Loggers::iterator itor = m_logs.find(logname);
		if(itor != m_logs.end()){
			lg = itor->second.getLogger(level);
		}else{
			lg = m_default_Logger.getLogger(logname, level); 
		}
		//mutexGive(&m_cs);
		lg.setHeader();

		return lg;
	}
		
	void SimpleLog::setPath(const std::string& p){
		if(m_appender){
			delete m_appender;
			m_appender = NULL;
		}

		if(p.size()){
			m_appender = new FileAppender(p, FileAppender::HOUR);
		}else{
			m_appender = new ConsoleAppender(); 
		}
		
		m_log.setAppender(m_appender);
	}

	void SimpleLog::setName(const std::string& n){
		m_log.setName(n);
	}

	void SimpleLog::setLevel(int level){
		m_log.setLevel(level);
	}	

	Logger SimpleLog::operator()(int32 level){
		return m_log.getLogger(level);		
	}


	FileAppender::FileAppender(const std::string& path, int32 schedu_span,
			bool immediately_flush, int32 max_cache_size)
		:m_path(path), m_cache_idx(0), m_schedu_span(schedu_span), 
		m_last_flush_time(0), m_last_open_time(0),
		m_file(NULL), m_immediately_flush(immediately_flush),
		m_max_cache_size(max_cache_size), m_flushlock(0){
		mutexInit(&m_cs);
	}

	FileAppender::~FileAppender(){
		std::cout << "FileAppender::~FileAppender\n";
		mutexDestroy(&m_cs); 
		flush_cache(m_caches + m_cache_idx % 2);
		flush_cache(m_caches + (1 + m_cache_idx) % 2);
		if(m_file)
			fclose(m_file);
	}

	int32 Appender::take(){
		atomicIncrement32(&m_refcnt);
		return 0;
	}

	int32 Appender::give(){
		atomicDecrement32(&m_refcnt);
		if(m_refcnt < 1){
			delete this;
		}

		return 0;
	}

	int32 FileAppender::take(){
		time_t n = time(NULL);
		time_t t = 0;
		atomicIncrement32(&m_refcnt);
		if(difftime(n, m_last_open_time) < m_schedu_span){
			return 0;
		}

		//open new file
		std::string filename;
		switch(m_schedu_span){
			case MINUTE:
				filename = m_path + "." + getStrMinute(n);
				t = getMinuteTimestamp(n);
				break;
			case HOUR:
				filename = m_path + "." + getStrHour(n);
				t = getHourTimestamp(n);
				break;
			case DAY:
			default:
				filename = m_path + "." + getStrDay(n);
				t = getDayTimestamp(n);
				break;
		}
		mutexTake(&m_cs);
		if(lock()){	
			if(m_file){
				fclose(m_file);
			}
			m_file = fopen(filename.c_str(), "ab+"); 
			m_last_open_time = t;
			unlock();
		}
		mutexGive(&m_cs);	
		return 0;
	}

	bool FileAppender::lock(){
		if(m_immediately_flush)
			return true;
		return atomicCompareExchange32(&m_flushlock, 1, 0) == 0;
	}

	bool FileAppender::unlock(){
		return atomicExchange32(&m_flushlock, 0) == 1;
	}

	int32 FileAppender::flush_cache(std::string* cache){
		if(m_file){
			fwrite(cache->data(), cache->size(), 1, m_file);
			fflush(m_file);
		}
		//clear will not free memory
		cache->clear();
		m_last_flush_time = time(NULL);
		return 0;
	}

	int32 FileAppender::write_s(const std::string& s){
		if(m_immediately_flush){
			if(!m_file)
				return -1;
			mutexTake(&m_cs);
			if(m_file){
				fwrite(s.data(), s.size(), 1, m_file);
				fflush(m_file);
			}
			mutexGive(&m_cs);

			return 0;
		}

		bool flush = false;
		time_t n = time(NULL);
		mutexTake(&m_cs);
		std::string* cache = m_caches + m_cache_idx % 2;
		std::string* oldcache = cache;
		cache->append(s.data(), s.size());
		//lock must at last
		if(((int32)cache->size() >= m_max_cache_size 
			|| n - m_last_flush_time > 1) && lock()){
			flush = true;
			++m_cache_idx;
	
		}
		mutexGive(&m_cs);
		if(flush){	
			flush_cache(oldcache);
			unlock();
		}

		return 0;
	}
	static log g_log;

	log& getLog(){
		return g_log;
	}		

	Logger logDebug(const std::string& logname){
		return g_log(logname, LOG_LEVEL_DEBUG);
	}

	Logger logTrace(const std::string& logname){
		return g_log(logname, LOG_LEVEL_TRACE);
	}

	Logger logInfo(const std::string& logname){
		return g_log(logname, LOG_LEVEL_INFO);
	}

	Logger logWarn(const std::string& logname){
		return g_log(logname, LOG_LEVEL_WARN);
	}

	Logger logError(const std::string& logname){
		return g_log(logname, LOG_LEVEL_ERROR);
	}

}
