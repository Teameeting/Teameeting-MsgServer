#include "log_init.h"
#include "json/json.h"
#include <stdlib.h>
#include <string.h>
#include <fstream>

namespace gim{
using namespace ef;

static int loadAppenders(log& l, const Json::Value& conf);
static int loadLoggers(log& l, const Json::Value& conf);
static int loadDefaultLogger(log& l, const Json::Value& conf);

int logInit(const std::string& config){

	int ret = 0;
	std::fstream f;

	f.open(config.data(), std::ios::in);

	if(!f.is_open()){
		std::cout << "log <config:" << config << "> open config file fail\n";
		return -1;
	}

	log& l = getLog();	
	Json::Reader reader;
	Json::Value root;

	if(!reader.parse(f, root, false)){
		std::cout << "log <config:" << config << "> parse config file fail\n";
		return -1;
	}
		
	ret = loadAppenders(l, root); 
	if(ret < 0){
		std::cout << "log <config:" << config << "> load appenders fail\n";
		return ret;
	}

	ret = loadLoggers(l, root);
	if(ret < 0){
		std::cout << "log <config:" << config << "> load loggers fail\n";
		return ret;
	}

	ret = loadDefaultLogger(l, root);
	if(ret < 0){
		std::cout << "log <config:" << config << "> load default logger fail\n";
	}

	return ret;
}

inline int getSchedeSpan(const char* s){
	int sspn = FileAppender::DAY; 

	if(!s){
		return sspn;
	}

	if(strncasecmp(s, "MINUTE", 6) == 0){
		sspn = FileAppender::MINUTE;
	}else if(strncasecmp(s, "HOUR", 4) == 0){
		sspn = FileAppender::HOUR;
	}else if(strncasecmp(s, "DAY", 3) == 0){
		sspn = FileAppender::DAY;
	} 

	return sspn;
}

inline bool isImmediaFlush(const char* f){
	bool immediately_flush = false;
	
	if(!f){
		return false;
	}

	if(strncasecmp(f, "TRUE", 4) == 0){
		immediately_flush = true;
	}
	
	return immediately_flush;
}

int loadFileAppender(log& l, const char* name, const Json::Value& conf){

	const Json::Value& p = conf["Path"];

	if(!p.isString()){
		return -10;
	}

	const char* path = p.asCString();

	if(!path){
		return -11;
	}

	const char* schedu_span = NULL; 
	const Json::Value& s = conf["ScheduSpan"];

	if(s.isString()){
		schedu_span = s.asCString();
	}

	int sspn = getSchedeSpan(schedu_span);
	bool immediately_flush = false;
	const char* immediately_flush_s = NULL;
	const Json::Value& f = conf["ImmediatelyFlush"];

	if(f.isString()){
		immediately_flush_s = f.asCString();
	}

	immediately_flush = isImmediaFlush(immediately_flush_s);
	l.addAppender(name, path, sspn, immediately_flush);

	return 0;

}


int loadAppender(log& l, const Json::Value& conf){

	int ret = 0;	

	if(!conf.isObject()){
		return -1;
	}

	const Json::Value& t = conf["Type"];
	if(!t.isString()){
		return -4;
	}

	const char* type = t.asCString();;
	if(!type){
		ret = -5;
	} 

	const Json::Value& n = conf["Name"];
	if(!n.isString()){
		return -2;
	}

	const char* name = n.asCString();
	if(!name){
		return -3;
	}


	if(strncasecmp(type, TYPE_FILE_APPENDER, 
		strlen(TYPE_FILE_APPENDER)) == 0){

		ret = loadFileAppender(l, name, conf);
		std::cout << "log <config:" << conf << "> load <appender:"
				<< name << "> <type:FileAppender> <ret:"
				<< ret << ">\n";

	}else if(strncasecmp(type, TYPE_CONSOLE_APPENDER, 
		strlen(TYPE_FILE_APPENDER)) == 0){
		l.addAppender(name);
		std::cout << "log <config:" << conf << "> load <appender:"
				 << name << "> <type:ConsoleAppender>\n";

	}else{
		std::cout << "log <config:" << conf << "> unsupport <appender:"
				 << name << "> <type:" << type << ">\n";
	}
	
	return ret;
}

int loadAppenders(log& l, const Json::Value& conf){

	int ret = 0;
	   
	const Json::Value& apds = conf["Appenders"];
	
	if(!apds.isArray()){
		std::cout << "log config get appenders fail\n";
		return -1;
	}

	for(Json::Value::const_iterator itr = apds.begin(); 
		itr != apds.end(); ++itr){
		ret = loadAppender(l, *itr);
		if(ret < 0)
			break;
	}

	return ret;	 
}

int getLoggerSetting(const Json::Value& conf, int& lv, std::string& append){

	const char* level = NULL;
	const Json::Value& l = conf["Level"];

	if(l.isString()){
		level = l.asCString();
	}

	lv = getStrLevel(level);

	const Json::Value& a = conf["Appender"];

	if(a.isString()){
		append = a.asString();
	}else{
		return -20;
	}
	
	return 0;
}

int loadLogger(log& l, const Json::Value& conf){

	int ret = 0;
	
	const Json::Value& n = conf["Name"];
	const char* name = NULL;
	
	if(!n.isString()){
		return -30;
	}

	name = n.asCString();

	if(!name){
		return -31;
	}

	int lv = LOG_LEVEL_NOT; 
	std::string append; 
	ret = getLoggerSetting(conf, lv, append);

	if(ret >=0){
		l.addLogger(name, lv, append);
	}	

	std::cout << "log load <logger:"
		<< name << "> <appender:" << append << "> <level:"
		<< lv << "> <ret: "<< ret << ">\n"; 

	return ret;

}

int loadLoggers(log& l, const Json::Value& conf){
	
	int ret = 0;
	
	const Json::Value& lgs = conf["Logs"];
	
	if(!lgs.isArray()){
		std::cout << "log config get logs fail\n";
		return -1;
	}

	for(Json::Value::const_iterator itr = lgs.begin(); 
		itr != lgs.end(); ++itr){
		ret = loadLogger(l, *itr);
		if(ret < 0)
			break;
	}

	return ret;
}

int loadDefaultLogger(log& l, const Json::Value& conf){
	
	int ret = 0;

	const Json::Value& dl = conf["DefaultLog"];
	if(dl.isNull()){
		std::cout << "log has no default logger\n"; 
		ret =0;
	}

	int lv = LOG_LEVEL_NOT; 
	std::string append; 
	ret = getLoggerSetting(dl, lv, append);

	if(ret >= 0){
		l.setDefaultLogger(lv, append); 
	}

	std::cout << "log load default_logger <appender:" 
		<< append << "> <level:" << lv 
		<< "> <ret:" << ret << ">\n"; 
	return ret;
}
 

}
