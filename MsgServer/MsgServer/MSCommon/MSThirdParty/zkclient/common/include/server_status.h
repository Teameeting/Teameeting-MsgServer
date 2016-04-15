#ifndef __SERVER_STATUS_H__
#define __SERVER_STATUS_H__

#include <vector>
#include <map>
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"
#include "rapidjson/reader.h"

namespace gim{


struct ServerStatus{
	std::string IP;
	std::string Type;
	std::string NodePath;
	std::string ModulePath;
	std::string ProjectPath;
	std::vector<std::string> IPs;

	int autoSetIPs(bool need_public_ip = true);
	int parseFromString(const std::string& v);
	int serializeToString(std::string& v) const;
};

typedef std::map<std::string/*id*/, ServerStatus>  SvStatusMap;


class ZKClient;

class ServerNode
{
public:
	friend class ZKClient;
	ServerNode(ZKClient* c, const std::string& typepath, const std::string& id);
	~ServerNode();
	int init(const ServerStatus& s);
	int setStatus(const ServerStatus& s);
private:
	int setData();
private:
	ZKClient* m_cli;
	std::string m_path;
	std::string m_data;
};

}

#endif/*__SERVER_STATUS_H__*/
