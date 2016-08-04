//
//  GRTConnManager.cpp
//  dyncRTGrouper
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "GRTConnManager.h"
#include <algorithm>
#include <assert.h>
#include <json/json.h>
#include "OSMutex.h"
#include "GRTTransferSession.h"
#include "RTZKClient.hpp"
#include "ProtoCommon.h"

static OSMutex       s_mutex;
static OSMutex       s_mutexModule;
static OSMutex       s_mutexTypeModule;

static GRTConnManager::ModuleInfoMaps                 s_ModuleInfoMap(0);
static GRTConnManager::TypeModuleSessionInfoLists     s_TypeModuleSessionInfoList(0);


GRTConnManager::ModuleInfo* GRTConnManager::findConnectorInfo(const std::string& userid)
{
    return findModuleInfo(userid, pms::ETransferModule::MCONNECTOR);
}

GRTConnManager::ModuleInfo* GRTConnManager::findModuleInfo(const std::string& uid, pms::ETransferModule module)
{
    GRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        GRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if (it->second && it->second->othModuleType == module) {
                pInfo = it->second;
                break;
            }
        }
    }
    return pInfo;
}

GRTConnManager::ModuleInfo* GRTConnManager::findModuleInfoBySid(const std::string& sid)
{

    GRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        GRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
        if (it!=s_ModuleInfoMap.end()) {
            pInfo = it->second;
        }
    }
    return pInfo;
}

GRTConnManager::ModuleInfo* GRTConnManager::findConnectorInfoById(const std::string& userid, const std::string& connector)
{

    if (userid.length()==0 || connector.length()==0) {
        LE("findConnectorInfoById userid or connector is 0\n");
        return NULL;
    }
    GRTConnManager::ModuleInfo* pInfo = NULL;
    std::string sessionid;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        TypeModuleSessionInfo* t_pInfo = NULL;
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            t_pInfo = *it;
            if (t_pInfo && t_pInfo->moduleId.compare(connector) == 0) {
                if (t_pInfo->sessionIds.empty()) {
                    continue;
                }
                sessionid = *(t_pInfo->sessionIds.begin());
                break;
            }
        }
    }
    if (sessionid.length()>0) {
        pInfo = findModuleInfoBySid(sessionid);
    } else {
        LE("findConnectorInfoById sessionid is null\n");
    }
    return pInfo;
}

bool GRTConnManager::ConnectGroupMgr()
{
    if (m_groupMgrAddrList.size() == 0) {
        LE("GRTConnManager::ConnectGroupMgr addr list.size is 0\n");
        return false;
    }
    if (m_pConnDispatcher==NULL)
        m_pConnDispatcher = new GRTConnDispatcher();
    std::list<std::string>::iterator it;
    for (it=m_groupMgrAddrList.begin(); it!=m_groupMgrAddrList.end(); it++) {
        std::string s = *it;
        char ip[16] = {0};
        unsigned int port = 0;
        sscanf(s.c_str(), "%s %u", ip, &port);
        LI("ip:%s, port:%u\n", ip, port);
        if (strlen(ip)>0 && port > 2048) {
            DoConnectGroupMgr(ip, port);
        }
    }
    return true;
}

bool GRTConnManager::DoConnectGroupMgr(const std::string ip, unsigned short port)
{
    GRTTransferSession* groupMgrSession = new GRTTransferSession();
    groupMgrSession->Init();
    // conn to connector
    while (!groupMgrSession->Connect(ip, port)) {
        LI("connecting to RTLive groupMgr server %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, port, groupMgrSession->GetSocket()->GetSocketFD());
    groupMgrSession->EstablishConnection();
    return true;
}

bool GRTConnManager::TryConnectGroupMgr(const std::string ip, unsigned short port)
{
    LI("GRTConnManager::TryConneectGroupMgr ip:%s, port:%u\n", ip.c_str(), port);
    GRTTransferSession* groupMgrSession = new GRTTransferSession();
    groupMgrSession->Init();
    // conn to connector

    bool ok = false;
    int times = 0;
    do{
        ok = groupMgrSession->Connect(ip, port);
        LI("try %d times to connect groupMgr server %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        groupMgrSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(groupMgrSession);
        if (m_pConnDispatcher)
            m_pConnDispatcher->Signal(Task::kIdleEvent);
        return false;
    }
}

void GRTConnManager::RefreshConnection()
{
    ModuleInfo* pmi = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            pmi = it->second;
            if (pmi && pmi->othModuleType == pms::ETransferModule::MCONNECTOR) {
                if (pmi->pModule && pmi->pModule->RefreshTime()) {
                    pmi->pModule->KeepAlive();
                }
            }
        }
    }
}

void GRTConnManager::GetGroupMembers(const std::string& groupid)
{

}

bool GRTConnManager::SignalKill()
{
    {
        OSMutexLocker mlocker(&s_mutexModule);
        for (auto & x : s_ModuleInfoMap) {
            x.second->pModule->Signal(Task::kKillEvent);
            usleep(100*1000);
        }
    }

    return true;
}

bool GRTConnManager::ClearAll()
{
    if (m_pConnDispatcher)
        m_pConnDispatcher->Signal(Task::kKillEvent);
    m_pConnDispatcher = nullptr;
    {
        OSMutexLocker mlocker(&s_mutexModule);
        for (auto & x : s_ModuleInfoMap) {
            delete x.second;
            x.second = NULL;
            usleep(100*1000);
        }
        s_ModuleInfoMap.clear();
    }

    {
        OSMutexLocker tlocker(&s_mutexTypeModule);
        for (auto & x : s_TypeModuleSessionInfoList) {
            delete x;
            x = NULL;
        }
        s_TypeModuleSessionInfoList.clear();
    }
    m_groupMgrAddrList.clear();
     return true;
}

bool GRTConnManager::AddModuleInfo(GRTConnManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    GRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        GRTConnManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool GRTConnManager::DelModuleInfo(const std::string& sid, EventData& data)
{
    OSMutexLocker locker(&s_mutexModule);
    GRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        GRTConnManager::ModuleInfo *p = it->second;
        data.connect.module = (int)p->othModuleType;
        memset(data.connect.ip, 0x00, 17);
        memcpy(data.connect.ip, p->pModule->GetTransferAddr().c_str(), p->pModule->GetTransferAddr().length());
        data.connect.port = p->pModule->GetTransferPort();

        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    return true;
}

bool GRTConnManager::AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid)
{
    TypeModuleSessionInfo* pInfo = NULL;
    bool found = false;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            if ((*it) && (*it)->moduleId.compare(mid) == 0) {
                pInfo = *it;
                found = true;
                break;
            }
        }
        if (found) {
            pInfo->sessionIds.insert(sid);
        } else {
            pInfo = new TypeModuleSessionInfo();
            pInfo->moduleType = module;
            pInfo->moduleId = mid;
            pInfo->sessionIds.insert(sid);
            s_TypeModuleSessionInfoList.push_front(pInfo);
        }
    }
    return true;
}

bool GRTConnManager::DelTypeModuleSession(const std::string& sid)
{
    TypeModuleSessionInfo* pInfo = NULL;
    bool found = false;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            pInfo = *it;
            std::set<std::string>::iterator sit = std::find(pInfo->sessionIds.begin(), pInfo->sessionIds.end(), sid);
            if (sit!=pInfo->sessionIds.end()) {
                pInfo->sessionIds.erase(sit);
                found = true;
            }
        }
    }
    return found;
}

void GRTConnManager::TransferSessionLostNotify(const std::string& sid)
{
    EventData data;
    data.mtype = SESSEVENT::_sess_lost;
    DelModuleInfo(sid, data);
    DelTypeModuleSession(sid);

#ifdef AUTO_RECONNECT
    //fire an event to restart
    {
        Json::Value request;
        request["type"] = data.mtype;
        request["module"] = data.connect.module;
        request["ip"] = data.connect.ip;
        request["port"] = data.connect.port;
        std::string s = request.toStyledString();
        LI("TransferSessionLostNotify EventData mtype:%d, module:%d, ip:%s, port:%d\n", data.mtype, data.connect.module, data.connect.ip, data.connect.port);
        LI("TransferSessionLostNotify s:%s\n", s.c_str());
        RTEventTimer* timer = new RTEventTimer(RETRY_MAX_TIME, &GRTConnManager::DispTimerCallback);
        timer->DataDelay(s.c_str(), (int)s.length());
        LI("Waiting for Session Reconnecting...");
    }
#endif
}

void GRTConnManager::OnTLogin(const std::string& uid, const std::string& token, const std::string& connector)
{

}

void GRTConnManager::OnTLogout(const std::string& uid, const std::string& token, const std::string& connector)
{

}

void GRTConnManager::ProcessRecvEvent(const char*pData, int nLen)
{
    if (!pData || nLen<=0) {
        return;
    }

    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(pData, root, false)) {
        LE("reader.parse error\n");
        return ;
    }
    EventData data;
    if (!root["type"].isInt()) {
        return;
    }
    data.mtype = root["type"].asInt();
    if (data.mtype == SESSEVENT::_sess_lost) {//offline and reconnect
        if (!root["module"].isInt() || !root["ip"].isString() || !root["port"].isInt()) {
            return;
        }
        data.connect.module = root["module"].asInt();
        memset(data.connect.ip, 0x00, 17);
        memcpy(data.connect.ip, root["ip"].asString().c_str(), (int)root["ip"].asString().length());
        data.connect.port = root["port"].asInt();
        LI("OnReadEvent EventData mtype:%d, module:%d, ip:%s, port:%d\n", data.mtype, data.connect.module, data.connect.ip, data.connect.port);
        if (data.connect.module == pms::ETransferModule::MGRPNOTIFY) {// connect to connector
            TryConnectGroupMgr(data.connect.ip, data.connect.port);
        }
    }
}

void GRTConnManager::ProcessTickEvent(const char*pData, int nLen)
{
    for(auto & x : m_connectingSessList) {
        if (x->GetConnectingStatus()==0) {
            bool ok = false;
            int times = 0;
            do{
                ok = x->Connect();
                usleep(2000*1000);
            }while(!ok && ++times < 5);
            if (m_pConnDispatcher)
                m_pConnDispatcher->Signal(Task::kIdleEvent);
        } else if (x->GetConnectingStatus() == 1) {
            x->EstablishConnection();
            m_connectingSessList.remove(x);
        }
    }
}

void GRTConnManager::PostDataStatic(const char* pData, int nLen)
{
    if (m_pConnDispatcher)
        m_pConnDispatcher->PostData(pData, nLen);
}

int GRTConnManager::DispTimerCallback(const char*pData, int nLen)
{
    if (pData && nLen>0) {

        Json::Reader reader;
        Json::Value root;
        if (!reader.parse(pData, root, false)) {
            LE("reader.parse error\n");
            return -1;
        }
        if (!root["type"].isInt() || !root["module"].isInt() || !root["ip"].isString() || !root["port"].isInt()) {
            return -1;
        }
        if (root["type"].asInt() == SESSEVENT::_sess_lost) {//offline and reconnect
            std::string s("");
            if (root["module"].asInt()== pms::ETransferModule::MGRPNOTIFY) {// connect to connector
                s = "/dync/msgserver/grouper/" + root["ip"].asString();
            } else {
                return 0;
            }
            if (s.length()>0) {
                if (RTZKClient::Instance().CheckNodeExists(s)) {
                    GRTConnManager::Instance().PostDataStatic(pData, nLen);
                } else {
                    RTEventTimer* timer = new RTEventTimer(RETRY_MAX_TIME, &GRTConnManager::DispTimerCallback);
                    timer->DataDelay(pData, nLen);
                }
            }
        }
    }
    return 0;
}



