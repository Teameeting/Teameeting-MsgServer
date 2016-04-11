//
//  CRTConnManager.cpp
//  dyncRTConnector
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTConnManager.h"
#include <assert.h>
#include "OSMutex.h"
#include <algorithm>

static OSMutex          s_mutex;
static OSMutex          s_mutexModule;
static OSMutex          s_mutexConnection;
static OSMutex          s_mutexTypeModule;

static CRTConnManager::ModuleInfoMaps               s_ModuleInfoMap(0);
static CRTConnManager::ConnectionInfoMaps           s_ConnectionInfoMap(0);
static CRTConnManager::TypeModuleSessionInfoLists   s_TypeModuleSessionInfoList(0);
static CRTConnManager::UserSessionInfoLists         s_UserSessionInfoList(0);
static CRTConnManager::UserSessionInfoMaps          s_UserSessionInfoMap(0);


CRTConnManager::ModuleInfo* CRTConnManager::findModuleInfo(const std::string& userid, TRANSFERMODULE module)
{
    CRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        CRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if (it->second && it->second->othModuleType == module) {
                pInfo = it->second;
                break;
            }
        }
    }
    return pInfo;
}

CRTConnManager::ConnectionInfo* CRTConnManager::findConnectionInfoById(const std::string& uid)
{
    CRTConnManager::ConnectionInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexConnection);
        CRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            pInfo = it->second;
        }
    }
    return pInfo;
}

bool CRTConnManager::AddModuleInfo(CRTConnManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    CRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        CRTConnManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool CRTConnManager::DelModuleInfo(const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    CRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        CRTConnManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    return true;
}

bool CRTConnManager::AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid)
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
            pInfo->moduleType = mtype;
            pInfo->moduleId = mid;
            pInfo->sessionIds.insert(sid);
            s_TypeModuleSessionInfoList.push_front(pInfo);
        }
    }
    return true;
}

bool CRTConnManager::DelTypeModuleSession(const std::string& sid)
{
    bool found = false;
    TypeModuleSessionInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            pInfo = *it;
            std::set<std::string>::iterator sit = std::find(pInfo->sessionIds.begin(), pInfo->sessionIds.end(), sid);
            if (sit!=pInfo->sessionIds.end()) {
                pInfo->sessionIds.erase(sit);
                found = true;
                break;
            }
        }
    }
    return found;
}

bool CRTConnManager::AddUser(CONNECTIONTYPE type, const std::string& uid, CRTConnManager::ConnectionInfo* pInfo)
{
    if (uid.length()==0 || !pInfo) return false;
    {
        OSMutexLocker locker(&s_mutexConnection);
        CRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            ConnectionLostNotify(uid, it->second->_token);
            delete it->second;
            it->second = NULL;
            s_ConnectionInfoMap.erase(uid);
        }
        s_ConnectionInfoMap.insert(make_pair(uid, pInfo));
    }
    return true;
}

bool CRTConnManager::DelUser(CONNECTIONTYPE type, const std::string& uid, std::string& token)
{
    if (uid.length()==0) return false;
    {
        OSMutexLocker locker(&s_mutexConnection);
        CRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            token = it->second->_token;
            delete it->second;
            it->second = NULL;
            s_ConnectionInfoMap.erase(uid);
            return true;
        } else {
            return false;
        }
    }
}

void CRTConnManager::ConnectionLostNotify(const std::string& uid, const std::string& token)
{
    ModuleInfo* pmi = findModuleInfo(uid, TRANSFERMODULE::mmeeting);
    if (pmi && pmi->pModule) {
        LI("CRTConnManager::ConnectionLostNotify uid:%s\n", uid.c_str());
        pmi->pModule->ConnectionLostNotify(uid, token);
    } else {
        LE("CRTConnManager::ConnectionLostNotify meeting pmi->pModule is NULL\n");
    }
    pmi = findModuleInfo(uid, TRANSFERMODULE::mmsgqueue);
    if (pmi && pmi->pModule) {
        LI("CRTConnManager::ConnectionLostNotify uid:%s\n", uid.c_str());
        pmi->pModule->ConnectionLostNotify(uid, token);
    } else {
        LE("CRTConnManager::ConnectionLostNotify msgqueue pmi->pModule is NULL\n");
    }
}

void CRTConnManager::ConnectionConnNotify(const std::string& uid, const std::string& token)
{
    ModuleInfo* pmi = findModuleInfo(uid, TRANSFERMODULE::mmeeting);
    if (pmi && pmi->pModule) {
        LI("CRTConnManager::ConnectionConnNotify uid:%s\n", uid.c_str());
        pmi->pModule->ConnectionConnNotify(uid, token);
    } else {
        LE("CRTConnManager::ConnectionConnNotify meeting pmi->pModule is NULL\n");
    }
    pmi = findModuleInfo(uid, TRANSFERMODULE::mmsgqueue);
    if (pmi && pmi->pModule) {
        LI("CRTConnManager::ConnectionConnNotify uid:%s\n", uid.c_str());
        pmi->pModule->ConnectionConnNotify(uid, token);
    } else {
        LE("CRTConnManager::ConnectionConnNotify msgqueue pmi->pModule is NULL\n");
    }
}

void CRTConnManager::TransferSessionLostNotify(const std::string& sid)
{
    DelModuleInfo(sid);
    DelTypeModuleSession(sid);
}

void CRTConnManager::TransferMsg(MSGTYPE mType, long long mseq, const std::string& uid, const std::string& msg)
{
    ModuleInfo* pmi = findModuleInfo(uid, (TRANSFERMODULE)mType);
    if (pmi && pmi->pModule) {
        pmi->pModule->TransferMsg(msg);
    } else {
        LE("pmi->pModule is NULL\n");
        Assert(false);
        return;
    }
}


bool CRTConnManager::SignalKill()
{
    {
        OSMutexLocker mlocker(&s_mutexModule);
        for (auto & x : s_ModuleInfoMap) {
            x.second->pModule->Signal(Task::kKillEvent);
            usleep(100*1000);
        }
    }

    {
        OSMutexLocker clocker(&s_mutexConnection);
        for (auto & x : s_ConnectionInfoMap) {
            x.second->_pConn->Signal(Task::kKillEvent);
            usleep(100*1000);
        }
    }

    return true;
}

bool CRTConnManager::ClearAll()
{
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

    {
        OSMutexLocker clocker(&s_mutexConnection);
        for (auto & x : s_ConnectionInfoMap) {
            delete x.second;
            x.second = NULL;
            usleep(100*1000);
        }
        s_ConnectionInfoMap.clear();
    }
     return true;
}


