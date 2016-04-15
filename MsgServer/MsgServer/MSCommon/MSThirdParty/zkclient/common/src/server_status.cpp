#include "server_status.h"
#include <sstream>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "net/ef_sock.h"
#include "zk_client.h"

namespace gim{

using namespace ef;

static int getPublicIPFromShell(std::string& ip){
	FILE* f = NULL;
	char buf[1024] = {0};

	f = popen("curl ipinfo.io", "r");

	if(!f){
		return -1;
	}

	fread(buf, sizeof(buf), 1, f);
	pclose(f);

    rapidjson::Document		jsonReqDoc;
    if (jsonReqDoc.ParseInsitu<0>((char*)buf).HasParseError())
    {
        fprintf(stderr, "getPublicIPFromShell parse ip error");
        return -1;
    }
    if(!(jsonReqDoc.HasMember("ip") && jsonReqDoc["ip"].IsString()))
    {
        return -1;
    }
    ip = jsonReqDoc["ip"].GetString();

    return 0;
}

int ServerStatus::autoSetIPs(bool need_public_ip){

	int ret = 0;

    std::string pubip;
	ret = getPublicIPFromShell(pubip);

	if(ret < 0){
		return ret;
	}

    std::vector<std::string> ips;
	getIPs(ips);

	for(size_t i = 0; i < ips.size(); ++i){
		if(ips[i] == pubip){
			continue;
		}
		IPs.push_back(ips[i]);
	}

	if(need_public_ip){
		IPs.push_back(pubip);
	}

	return ret;
}


int ServerStatus::parseFromString(const std::string& s){

    rapidjson::Document		root;
    if (root.ParseInsitu<0>((char*)s.c_str()).HasParseError())
    {
        fprintf(stderr, "parseFromString error");
        return -1;
    }
    if(!(root.HasMember("IP") && root["IP"].IsString()))
    {
        return -1;
    }
    IP = root["IP"].GetString();

    if(!(root.HasMember("Type") && root["Type"].IsString()))
    {
        return -1;
    }
    Type = root["Type"].GetString();

    if(!(root.HasMember("NodePath") && root["NodePath"].IsString()))
    {
        return -1;
    }
    NodePath = root["NodePath"].GetString();

    if(!(root.HasMember("ModulePath") && root["ModulePath"].IsString()))
    {
        return -1;
    }
    ModulePath = root["ModulePath"].GetString();

    if(!(root.HasMember("ProjectPath") && root["ProjectPath"].IsString()))
    {
        return -1;
    }
    ProjectPath = root["ProjectPath"].GetString();


    if(!(root.HasMember("IPs") && root["IPs"].IsArray()))
    {
        return -1;
    }
    rapidjson::Value& mems = root["IPs"];
    for (int i=0; i<(int)mems.Capacity(); i++) {
        rapidjson::Value& m = mems[i];
        IPs.push_back(m.GetString());
    }

	return -1;
}

int ServerStatus::serializeToString(std::string& s) const {
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("IP", IP.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("Type", Type.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("NodePath", NodePath.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ModulePath", ModulePath.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ProjectPath", ProjectPath.c_str(), jDoc.GetAllocator());

    rapidjson::Value mems(rapidjson::kArrayType);
    std::vector<std::string>::const_iterator it = IPs.begin();
    for (; it!=IPs.end(); it++) {
        mems.PushBack((*it).c_str(), jDoc.GetAllocator());
    }
    jDoc.AddMember("IPs", mems, jDoc.GetAllocator());

    jDoc.Accept(writer);
    s = sb.GetString();
	return 0;
}

ServerNode::ServerNode(ZKClient* c, const std::string& typepath, const std::string& id)
:m_cli(c)
{
	m_path = zkPath(typepath, id);
	if (c)
		c->addSrvNode(this);
}

ServerNode::~ServerNode()
{
	if (m_cli){
		m_cli->deleteNode(m_path);
		m_cli->delSrvNode(this);
	}
}
int ServerNode::init(const ServerStatus& s)
{
	if (m_cli){
		if (s.serializeToString(m_data) == 0){
			return m_cli->createEphemeralNode(m_path, m_data);
		}
	}

	return -1;
}

int ServerNode::setStatus(const ServerStatus& s){
    std::string data;
    s.serializeToString(data);

	if (m_data != data){
		m_data = data;
		return setData();
	}
	return 0;
}
int ServerNode::setData()
{
	int ret = -1;
	if (m_cli){
		ret = m_cli->setNodeData(m_path, m_data);
		if (ZNONODE == ret){
			ret = m_cli->createEphemeralNode(m_path, m_data);
		}
	}
	return ret;
}
};
