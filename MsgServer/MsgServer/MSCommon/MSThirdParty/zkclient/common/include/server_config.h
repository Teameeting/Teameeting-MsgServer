#ifndef __SERVER_CONFIG_H__
#define __SERVER_CONFIG_H__

#include <vector>
#include <json/json.h> 
#include "common.h"
namespace gim{

class ServerConfig{
public:
	std::string IP; 
	std::string Type;
	std::string ZkUrl;
	std::string NodePath;
	std::string ModulePath;
	std::string ProjectPath;

	std::string HttpIp;
	std::string LogPath;
	int Debug;
	int Level;

	PortConfig portConfig;

	ServerConfig():IP("")
		,Type("")
		,ZkUrl("")
		,NodePath("")
		,ModulePath("")
		,ProjectPath("")
		,HttpIp("")
		,LogPath("")
		,Debug(0)
		,Level(0){
	}

	int init(const std::string& f);

	int serializeToJson(Json::Value& v) const;
	int parseFromJson(const Json::Value& v);

	std::string toStyledString() const;

private:
	int loadPortsConfigs(const Json::Value& v);
	
};

}

#endif
