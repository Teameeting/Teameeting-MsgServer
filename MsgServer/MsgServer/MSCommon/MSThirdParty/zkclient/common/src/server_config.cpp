#include "server_config.h"
#include "base/ef_log.h"
#include <fstream>

namespace gim{


using namespace std;
using namespace ef;

int ServerConfig::init(const std::string& f){

	int ret = 0;
	fstream ifs;

	ifs.open(f.data(), ios::in);

	if(!ifs.is_open()){
		return -1;
	}

	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(ifs, root, false)) {
		printf("reader.parse error\n");
		return -2;
	}

	ret = parseFromJson(root);

	return ret;
}


int ServerConfig::parseFromJson(const Json::Value& root){

	int ret = 0;

	if (root["IP"].isString()) {
		IP = root["IP"].asString();
	}

	if (root["Type"].isString()) {
		Type = root["Type"].asString();
	}

	if (root["ZkUrl"].isString()) {
		ZkUrl = root["ZkUrl"].asString();
	}

	if (root["NodePath"].isString()) {
		NodePath = root["NodePath"].asString();
	}

	if (root["ModulePath"].isString()) {
		ModulePath = root["ModulePath"].asString();
	}

	if (root["ProjectPath"].isString()) {
		ProjectPath = root["ProjectPath"].asString();
	}

	if (root["HttpIp"].isString()) {
		HttpIp = root["HttpIp"].asString();
	}

	if (root["LogPath"].isString()) {
		LogPath = root["LogPath"].asString();
	}

	if (root["Debug"].isInt()) {
		Debug = root["Debug"].asInt();
	}

	if (root["Level"].isInt()) {
		Level = root["Level"].asInt();
	}

	ret = loadPortsConfigs(root["PortConfigs"]);

	return ret;
}

	
int ServerConfig::loadPortsConfigs(const Json::Value& v){

    const Json::Value& cf = v;
    
    const Json::Value& Type = cf["Type"];
    const Json::Value& Port = cf["Port"];
    
    if(!Type.isInt() || !Port.isObject()){
        printf("loadPortsConfigs error\n");
        return -1;
    }
    if (Type.asInt()==1) {
        const Json::Value& ListenWebcon = Port["ListenWebcon"];
        const Json::Value& ListenModule = Port["ListenModule"];
        const Json::Value& ListenClicon = Port["ListenClicon"];
        if(!ListenWebcon.isInt() || !ListenModule.isInt() || !ListenClicon.isInt()){
            return -1;
        }
        
        portConfig.MTYPE = Type.asInt();
        portConfig.connector.ListenWebcon = ListenWebcon.asInt();
        portConfig.connector.ListenModule = ListenModule.asInt();
        portConfig.connector.ListenClicon = ListenClicon.asInt();
    } else if (Type.asInt()==2) {
        const Json::Value& AcceptConn = Port["AcceptConn"];
        const Json::Value& ListenDisp = Port["ListenDisp"];
        const Json::Value& ListenHttp = Port["ListenHttp"];
        if(!AcceptConn.isInt() || !ListenDisp.isInt() || !ListenHttp.isInt()){
            return -1;
        }
        portConfig.MTYPE = Type.asInt();
        portConfig.dispatcher.AcceptConn = AcceptConn.asInt();
        portConfig.dispatcher.ListenDisp = ListenDisp.asInt();
        portConfig.dispatcher.ListenHttp = ListenHttp.asInt();
    } else if (Type.asInt()==3) {
        const Json::Value& AcceptConn = Port["AcceptConn"];
        const Json::Value& AcceptDisp = Port["AcceptDisp"];
        const Json::Value& AcceptHttp = Port["AcceptHttp"];
        if(!AcceptConn.isInt() || !AcceptDisp.isInt() || !AcceptHttp.isInt()){
            return -1;
        }
        portConfig.MTYPE = Type.asInt();
        portConfig.meeting.AcceptConn = AcceptConn.asInt();
        portConfig.meeting.AcceptDisp = AcceptDisp.asInt();
        portConfig.meeting.AcceptHttp = AcceptHttp.asInt();
    }
    
    return 0;
}


int ServerConfig::serializeToJson(Json::Value& v) const{
	v["IP"] = IP;
	v["Type"] = Type;
	v["ZkUrl"] = ZkUrl;
	v["NodePath"] = NodePath;
	v["ModulePath"] = ModulePath;
	v["ProjectPath"] = ProjectPath;
	v["LogPath"] = LogPath;
	v["Debug"] = Debug;
	v["Level"] = Level;

    PortConfig x = portConfig;
    Json::Value vv;
    if (x.MTYPE==1) {
        vv["Type"] = x.MTYPE;
        Json::Value vvv;
        vvv["ListenWebcon"] = x.connector.ListenWebcon;
        vvv["ListenModule"] = x.connector.ListenModule;
        vvv["ListenClicon"] = x.connector.ListenClicon;
        vv["Port"] = vvv;
        v["PortConfigs"] = vv;
    } else if (x.MTYPE==2) {
        vv["Type"] = x.MTYPE;
        Json::Value vvv;
        vvv["AcceptConn"] = x.dispatcher.AcceptConn;
        vvv["ListenDisp"] = x.dispatcher.ListenDisp;
        vvv["ListenHttp"] = x.dispatcher.ListenHttp;
        vv["Port"] = vvv;
        v["PortConfigs"] = vv;
    } else if (x.MTYPE==3) {
        vv["Type"] = x.MTYPE;
        Json::Value vvv;
        vvv["AcceptConn"] = x.meeting.AcceptConn;
        vvv["AcceptDisp"] = x.meeting.AcceptDisp;
        vvv["AcceptHttp"] = x.meeting.AcceptHttp;
        vv["Port"] = vvv;
        v["PortConfigs"] = vv;
    }
    
	return 0;
}

string ServerConfig::toStyledString() const{
	Json::Value v;
	serializeToJson(v);
	return v.toStyledString();
}

}
