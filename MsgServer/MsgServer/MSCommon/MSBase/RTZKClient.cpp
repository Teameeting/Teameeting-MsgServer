//
//  RTZKClient.cpp
//  MsgServer
//
//  Created by hp on 3/5/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include "RTZKClient.hpp"
#include <iostream>
#include "rtklog.h"


RTZKClient::RTZKClient()
: m_conf_path("")
, m_client(NULL)
, m_server_node(NULL)
{
    m_listWs.clear();
    m_dataWs.clear();
}

RTZKClient::~RTZKClient()
{
    Unin();
}

void RTZKClient::RTZKLogCallBack(void* ctx, const std::string& l)
{
    //LI("Log:%s\n", l.c_str());
}

int RTZKClient::RTZKDataCallback(void* ctx, int version, const std::string& data)
{
    return 0;
}

////////////
int RTZKClient::ChildrenMapCallback(const gim::ChildrenMap& cmap)
{
    gim::ListWatcher< RTZKClient>* w = NULL;
    for (auto& x : cmap) {
        gim::ServerStatus ss;
        ss.parseFromString(x.second);
        if (m_mapChildWs.find(ss.NodePath)==m_mapChildWs.end()) {
            w = new gim::ListWatcher< RTZKClient >(this, &RTZKClient::ChildrenMapCallback, &RTZKClient::PathRemoveCallback);
            w->init(m_client, ss.NodePath);
            LI("RTZKClient::ChildredMapCallback node %s was added\n", ss.NodePath.c_str());
            m_mapChildWs.insert(make_pair(ss.NodePath, w));
        }
    }
    return 0;
}

int RTZKClient::PathRemoveCallback(const std::string& path)
{
    ChildrenWatcherMapIt it = m_mapChildWs.find(path);
    if (it!=m_mapChildWs.end()) {
        LI("RTZKClient::PathRemoveCallback node %s was removed\n", path.c_str());
        delete it->second;
        it->second = NULL;
        m_mapChildWs.erase(it);
    }
    return 0;
}

int RTZKClient::RTZKCallback(int ver, const std::string& data)
{
    return 0;
}

int RTZKClient::InitStatusNode(gim::ServerConfig& conf){

    m_server_status.IP = conf.IP;
    m_server_status.Type = conf.Type;
    m_server_status.NodePath = conf.NodePath;
    m_server_status.ModulePath = conf.ModulePath;
    m_server_status.ProjectPath = conf.ProjectPath;
    m_server_status.autoSetIPs(true);
    m_server_node = new gim::ServerNode(m_client, conf.ModulePath, conf.IP);
    int ret = m_server_node->init(m_server_status);
    if(ret < 0){
        LE("m_server_node->init failed\n");
        return -12;
    }
    return 0;
}

bool RTZKClient::CheckNodeExists(const std::string& nodePath)
{
    return ((m_mapChildWs.find(nodePath)==m_mapChildWs.end())?false:true);
}

int RTZKClient::InitZKClient(const std::string& conf)
{
    m_conf_path = conf;
    int res = m_conf.init(m_conf_path);
    if (res!=0) {
        LE("m_conf.init failed\n");
        return res;
    }

    m_client =  new gim::ZKClient();
    res = m_client->init(m_conf.ZkUrl);

    res = InitStatusNode(m_conf);
    if (res == ZOK) {
        LI("InitStatusNode path:%s ip:%s ok!\n", m_conf.ModulePath.c_str(), m_conf.IP.c_str());
    } else if (res == ZNODEEXISTS) {
        LE("InitStatusNode node exists path:%s ip:%s ok!\n", m_conf.ModulePath.c_str(), m_conf.IP.c_str());
        return res;
    } else {
        LE("InitStatusNode error, path:%s ip:%s ok!\n", m_conf.ModulePath.c_str(), m_conf.IP.c_str());
        return res;
    }
    m_client->setLogFn(this, &RTZKClient::RTZKLogCallBack);

    gim::ListWatcher< RTZKClient>* lw = NULL;
    gim::DataWatcher< RTZKClient>* dw = NULL;
    gim::s_vector c_v;

    //child watch
    m_client->getChildren(m_conf.ProjectPath, c_v);
    for(auto& x:c_v) {
        std::string s = gim::zkPath(m_conf.ProjectPath, x.c_str());
        lw = new gim::ListWatcher< RTZKClient >(this, &RTZKClient::ChildrenMapCallback, &RTZKClient::PathRemoveCallback);
        lw->init(m_client, s);
        lw->addWatch(gim::ZKWatcher::DELETE_EVENT);
        lw->addWatch(gim::ZKWatcher::CREATE_EVENT);
        m_listWs.push_back(lw);
    }

    //data watch
    dw = new gim::DataWatcher< RTZKClient >(this, &RTZKClient::RTZKCallback);
    dw->init(m_client, m_conf.ProjectPath);
    dw->addWatch(gim::ZKWatcher::CHANGE_EVENT);
    m_dataWs.push_back(dw);

    dw = new gim::DataWatcher< RTZKClient >(this, &RTZKClient::RTZKCallback);
    dw->init(m_client, m_conf.ModulePath);
    dw->addWatch(gim::ZKWatcher::CHANGE_EVENT);
    m_dataWs.push_back(dw);

    return 0;

}

int RTZKClient::InitOnly(const std::string& conf)
{
    m_conf_path = conf;
    return m_conf.init(m_conf_path);
}

int RTZKClient::Unin()
{
    for(auto& x : m_listWs){
        delete x;
        x = NULL;
    }
    m_listWs.clear();
    for(auto& x : m_dataWs){
        delete x;
        x = NULL;
    }
    m_dataWs.clear();
    for(auto x : m_mapChildWs){
        delete x.second;
        x.second = NULL;
    }
    m_mapChildWs.clear();
    if (m_server_node) {
         delete m_server_node;
         m_server_node = NULL;
    }
    if (m_client) {
        delete m_client;
        m_client = NULL;
    }
     return 0;
}
