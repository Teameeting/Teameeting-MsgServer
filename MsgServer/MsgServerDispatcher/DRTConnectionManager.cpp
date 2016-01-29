//
//  RTConnectionManager.cpp
//  dyncRTMsgQueue
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "DRTConnectionManager.h"
#include <algorithm>
#include <assert.h>
#include "OSMutex.h"
#include "DRTTransferSession.h"

static OSMutex       s_mutex;
static OSMutex       s_mutexModule;
static OSMutex       s_mutexTypeModule;

static DRTConnectionManager::ModuleInfoMaps                 s_ModuleInfoMap(0);
static DRTConnectionManager::TypeModuleSessionInfoLists     s_TypeModuleSessionInfoList(0);
static DRTConnectionManager::UserSessionInfoLists           s_UserSessionInfoList(0);
static DRTConnectionManager::UserSessionInfoMaps            s_UserSessionInfoMap(0);


DRTConnectionManager::ModuleInfo* DRTConnectionManager::findConnectorInfo(const std::string& userid)
{
    return findModuleInfo(userid, TRANSFERMODULE::mconnector);
}

DRTConnectionManager::ModuleInfo* DRTConnectionManager::findModuleInfo(const std::string& uid, TRANSFERMODULE module)
{
    DRTConnectionManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        if (s_ModuleInfoMap.size()==0) { return NULL; }
        DRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if (it->second && it->second->othModuleType == module) {
                pInfo = it->second;
                break;
            }
        }
    }
    return pInfo;
}

DRTConnectionManager::ModuleInfo* DRTConnectionManager::findModuleInfoBySid(const std::string& sid)
{
    
    DRTConnectionManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        if (s_ModuleInfoMap.size()==0) { return NULL; }
        DRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
        if (it!=s_ModuleInfoMap.end()) {
            pInfo = it->second;
        }
    }
    return pInfo;
}

DRTConnectionManager::ModuleInfo* DRTConnectionManager::findConnectorInfoById(const std::string& userid, const std::string& connector)
{
    
    if (userid.length()==0 || connector.length()==0) {
        if (userid.length()==0) {
            LE("findConnectorInfoById userid.length is 0\n");
        }
        if (connector.length()==0) {
            LE("findConnectorInfoById connector.length is 0\n");
        }
        return NULL;
    }
    DRTConnectionManager::ModuleInfo* pInfo = NULL;
    std::string sessionid;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        if (s_TypeModuleSessionInfoList.size()==0) { return NULL; }
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        TypeModuleSessionInfo* t_pInfo = NULL;
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            t_pInfo = *it;
            if (t_pInfo && t_pInfo->moduleId.compare(connector) == 0) {
                if (t_pInfo->sessionIds.empty()) {
                    continue;
                }
                sessionid = *(t_pInfo->sessionIds.begin());
                LI("find transfer sessionid:%s", sessionid.c_str());
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

bool DRTConnectionManager::ConnectConnector()
{
    if (m_ipList.size() == 0) {
        return false;
    }
    std::list<std::string>::iterator it;
    for (it=m_ipList.begin(); it!=m_ipList.end(); it++) {
        std::string s = *it;
        char ip[16] = {0};
        unsigned int port = 0;
        sscanf(s.c_str(), "%s %u", ip, &port);
        printf("ip:%s, port:%u\n", ip, port);
        if (strlen(ip)>0 && port > 2048) {
            DoConnectConnector(ip, port);
        }
    }
    return true;
}

bool DRTConnectionManager::DoConnectConnector(const std::string ip, unsigned short port)
{
    DRTTransferSession* connectorSession = new DRTTransferSession();
    connectorSession->Init();
    // conn to connector
    while (!connectorSession->Connect(ip, port)) {
        LI("connecting to connector server %s:%u waiting...\n", ip.c_str(), port);
        sleep(1);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, port, connectorSession->GetSocket()->GetSocketFD());
    connectorSession->EstablishConnection();
    return true;
}

void DRTConnectionManager::RefreshConnection()
{
    ModuleInfo* pmi = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        if (s_ModuleInfoMap.size()==0) { return ; }
        ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            pmi = it->second;
            if (pmi && pmi->othModuleType == TRANSFERMODULE::mconnector) {
                if (pmi->pModule && pmi->pModule->RefreshTime()) {
                    pmi->pModule->KeepAlive();
                }
            }
        }
    }
}

bool DRTConnectionManager::AddModuleInfo(DRTConnectionManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    if (s_ModuleInfoMap.size()==0) {
        s_ModuleInfoMap.insert(make_pair(sid, pmi));
        return true;
    }
    DRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        DRTConnectionManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool DRTConnectionManager::DelModuleInfo(const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    if (s_ModuleInfoMap.size()==0) { return true; }
    DRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        DRTConnectionManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    return true;
}

bool DRTConnectionManager::AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid)
{
    TypeModuleSessionInfo* pInfo = NULL;
    bool found = false;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        if (s_TypeModuleSessionInfoList.size()==0) {
            pInfo = new TypeModuleSessionInfo();
            pInfo->moduleType = mtype;
            pInfo->moduleId = mid;
            pInfo->sessionIds.insert(sid);
            s_TypeModuleSessionInfoList.push_front(pInfo);
            return true;
        }
    
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
            pInfo->moduleType = mtype;
            pInfo->moduleId = mid;
            pInfo->sessionIds.insert(sid);
            s_TypeModuleSessionInfoList.push_front(pInfo);
        }
    }
    return true;
}

bool DRTConnectionManager::DelTypeModuleSession(const std::string& sid)
{
    TypeModuleSessionInfo* pInfo = NULL;
    bool found = false;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        if (s_TypeModuleSessionInfoList.size()==0) { return false; }
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

void DRTConnectionManager::TransferSessionLostNotify(const std::string& sid)
{
    DelModuleInfo(sid);
    DelTypeModuleSession(sid);
    LI("RTConnectionManager::TransferSessionLostNotify sessionid:%s\n", sid.c_str());
}
