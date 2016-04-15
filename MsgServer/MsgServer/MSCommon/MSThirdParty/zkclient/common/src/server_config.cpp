#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "server_config.h"
#include "base/ef_log.h"

namespace gim{


using namespace std;
using namespace ef;

ServerConfig::ServerConfig()
    :IP("")
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

ServerConfig::~ServerConfig(){
}

int ServerConfig::init(const std::string& f){

	int ret = 0;
    FILE* pf = fopen(f.c_str(), "r");
    if (pf) {
        rapidjson::FileStream fs(pf);

        rapidjson::Document		jsonReqDoc;
        if (jsonReqDoc.ParseStream<0>(fs).HasParseError()) {
            fprintf(stderr, "jsonReqDoc.ParseStream error");
            fclose(pf);
            return -2;
        }
        ret = parseFromJson(jsonReqDoc);
        fclose(pf);
    }
	return ret;
}

int ServerConfig::parseFromJson(const rapidjson::Document& root){

	int ret = 0;

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

    if(!(root.HasMember("ZkUrl") && root["ZkUrl"].IsString()))
    {
        return -1;
    }
    ZkUrl = root["ZkUrl"].GetString();

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

    if(!(root.HasMember("HttpIp") && root["HttpIp"].IsString()))
    {
        return -1;
    }
    HttpIp = root["HttpIp"].GetString();

    if(!(root.HasMember("LogPath") && root["LogPath"].IsString()))
    {
        return -1;
    }
    LogPath = root["LogPath"].GetString();

    if(!(root.HasMember("Debug") && root["Debug"].IsInt()))
    {
        return -1;
    }
    Debug = root["Debug"].GetInt();

    if(!(root.HasMember("Level") && root["Level"].IsInt()))
    {
        return -1;
    }
    Level = root["Level"].GetInt();

	ret = loadPortsConfigs(root["PortConfigs"]);
    //printf("Ip:%s, type:%s, zkurl:%s, nodepath:%s, module:%s, projeect:%s, httpip:%s, logpath:%s, debug:%d, level:%d\n", IP.c_str(), Type.c_str(), ZkUrl.c_str(), NodePath.c_str()
    //        , ModulePath.c_str(), ProjectPath.c_str()
    //        , HttpIp.c_str(), LogPath.c_str()
    //        , Debug, Level);

	return ret;
}

int ServerConfig::loadPortsConfigs(const rapidjson::Value& v){

    const rapidjson::Value& cf = v;

    const rapidjson::Value& Type = cf["Type"];
    const rapidjson::Value& Port = cf["Port"];

    if(!Type.IsInt() || !Port.IsObject()){
        fprintf(stderr, "loadPortsConfigs error");
        return -1;
    }
    if (Type.GetInt()==1) {
        const rapidjson::Value& ListenWebcon = Port["ListenWebcon"];
        const rapidjson::Value& ListenModule = Port["ListenModule"];
        const rapidjson::Value& ListenClicon = Port["ListenClicon"];
        if(!ListenWebcon.IsInt() || !ListenModule.IsInt() || !ListenClicon.IsInt()){
            return -1;
        }

        portConfig.MTYPE = Type.GetInt();
        portConfig.connector.ListenWebcon = ListenWebcon.GetInt();
        portConfig.connector.ListenModule = ListenModule.GetInt();
        portConfig.connector.ListenClicon = ListenClicon.GetInt();
        //printf("connector type:%d, webcon:%d, module:%d, clicon:%d\n\n", portConfig.MTYPE, portConfig.connector.ListenWebcon
        //        , portConfig.connector.ListenModule, portConfig.connector.ListenClicon);
    } else if (Type.GetInt()==2) {
        const rapidjson::Value& AcceptConn = Port["AcceptConn"];
        const rapidjson::Value& ListenDisp = Port["ListenDisp"];
        const rapidjson::Value& ListenHttp = Port["ListenHttp"];
        if(!AcceptConn.IsInt() || !ListenDisp.IsInt() || !ListenHttp.IsInt()){
            return -1;
        }
        portConfig.MTYPE = Type.GetInt();
        portConfig.dispatcher.AcceptConn = AcceptConn.GetInt();
        portConfig.dispatcher.ListenDisp = ListenDisp.GetInt();
        portConfig.dispatcher.ListenHttp = ListenHttp.GetInt();
        //printf("dispatcher type:%d, webcon:%d, module:%d, clicon:%d\n\n", portConfig.MTYPE, portConfig.dispatcher.AcceptConn
        //        , portConfig.dispatcher.ListenDisp, portConfig.dispatcher.ListenHttp);
    } else if (Type.GetInt()==3) {
        const rapidjson::Value& AcceptConn = Port["AcceptConn"];
        const rapidjson::Value& AcceptDisp = Port["AcceptDisp"];
        const rapidjson::Value& AcceptHttp = Port["AcceptHttp"];
        if(!AcceptConn.IsInt() || !AcceptDisp.IsInt() || !AcceptHttp.IsInt()){
            return -1;
        }
        portConfig.MTYPE = Type.GetInt();
        portConfig.meeting.AcceptConn = AcceptConn.GetInt();
        portConfig.meeting.AcceptDisp = AcceptDisp.GetInt();
        portConfig.meeting.AcceptHttp = AcceptHttp.GetInt();
        //printf("meeting type:%d, webcon:%d, module:%d, clicon:%d\n\n", portConfig.MTYPE, portConfig.meeting.AcceptConn
        //        , portConfig.meeting.AcceptDisp, portConfig.meeting.AcceptHttp);
    }

    return 0;
}

string ServerConfig::toStyledString() const{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    jDoc.SetObject();
    jDoc.AddMember("IP", IP.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("Type", Type.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ZkUrl", ZkUrl.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("NodePath", NodePath.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ModulePath", ModulePath.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("ProjectPath", ProjectPath.c_str(), jDoc.GetAllocator());
    jDoc.AddMember("LogPath", LogPath.c_str(), jDoc.GetAllocator());

    PortConfig x = portConfig;
    rapidjson::Value vv;
    if (x.MTYPE==1) {
        vv["Type"] = x.MTYPE;
        rapidjson::Value vvv;
        vvv["ListenWebcon"] = x.connector.ListenWebcon;
        vvv["ListenModule"] = x.connector.ListenModule;
        vvv["ListenClicon"] = x.connector.ListenClicon;
        vv["Port"] = vvv;
    } else if (x.MTYPE==2) {
        vv["Type"] = x.MTYPE;
        rapidjson::Value vvv;
        vvv["AcceptConn"] = x.dispatcher.AcceptConn;
        vvv["ListenDisp"] = x.dispatcher.ListenDisp;
        vvv["ListenHttp"] = x.dispatcher.ListenHttp;
        vv["Port"] = vvv;
    } else if (x.MTYPE==3) {
        vv["Type"] = x.MTYPE;
        rapidjson::Value vvv;
        vvv["AcceptConn"] = x.meeting.AcceptConn;
        vvv["AcceptDisp"] = x.meeting.AcceptDisp;
        vvv["AcceptHttp"] = x.meeting.AcceptHttp;
        vv["Port"] = vvv;
    }
    jDoc.AddMember("PortConfigs", vv, jDoc.GetAllocator());

    jDoc.Accept(writer);
    std::string s = sb.GetString();

	return s;
}

}
