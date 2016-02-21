//
//  CRTConnectionManager.cpp
//  dyncRTConnector
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTConnectionManager.h"
#include <assert.h>
#include "OSMutex.h"
#include <algorithm>

std::string     CRTConnectionManager::s_cohttpIp;
unsigned short  CRTConnectionManager::s_cohttpPort;
std::string     CRTConnectionManager::s_cohttpHost;

static OSMutex          s_mutex;
static OSMutex          s_mutexModule;
static OSMutex          s_mutexConnection;
static OSMutex          s_mutexTypeModule;

static CRTConnectionManager::ModuleInfoMaps               s_ModuleInfoMap(0);
static CRTConnectionManager::ConnectionInfoMaps           s_ConnectionInfoMap(0);
static CRTConnectionManager::TypeModuleSessionInfoLists   s_TypeModuleSessionInfoList(0);
static CRTConnectionManager::UserSessionInfoLists         s_UserSessionInfoList(0);
static CRTConnectionManager::UserSessionInfoMaps          s_UserSessionInfoMap(0);


CRTConnectionManager::ModuleInfo* CRTConnectionManager::findModuleInfo(const std::string& userid, TRANSFERMODULE module)
{
    CRTConnectionManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        if (s_ModuleInfoMap.size()==0) { return NULL; }
        CRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        //LI("=====findModuleInfo module:%d, maps.size:%d, user:%s\n", (int)module, (int)s_ModuleInfoMap.size(), userid.c_str());
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if (it->second && it->second->othModuleType == module) {
                pInfo = it->second;
                LI("%s FIND info.moduletype:%d, module:%d\n", __FUNCTION__, pInfo->othModuleType, module);
                break;
            }
        }
    }
    return pInfo;
}

CRTConnectionManager::ConnectionInfo* CRTConnectionManager::findConnectionInfoById(const std::string& uid)
{
    CRTConnectionManager::ConnectionInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexConnection);
        if (s_ConnectionInfoMap.size()==0) { return NULL; }
        CRTConnectionManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            pInfo = it->second;
        } else {
            LI("findConnection after show NOOOOOOT find uid:%s\n", uid.c_str());
        }
    }
    return pInfo;
}

bool CRTConnectionManager::AddModuleInfo(CRTConnectionManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    if (s_ModuleInfoMap.size()==0) {
        s_ModuleInfoMap.insert(make_pair(sid, pmi));
        return true;
    }
    CRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        CRTConnectionManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
        LI("DelModuleInfo find and delete sid:%s\n", sid.c_str());
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool CRTConnectionManager::DelModuleInfo(const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    if (s_ModuleInfoMap.size()==0) { return true; }
    CRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        CRTConnectionManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
        LI("DelModuleInfo find and delete sid:%s\n", sid.c_str());
    }
    return true;
}

bool CRTConnectionManager::AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid)
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
                LI("AddTypeModuleSession find module size:%d, type:%d, mid:%s, sid:%s\n", (int)s_TypeModuleSessionInfoList.size(), (int)mtype, mid.c_str(), sid.c_str());
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

bool CRTConnectionManager::DelTypeModuleSession(const std::string& sid)
{
    bool found = false;
    TypeModuleSessionInfo* pInfo = NULL;
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
                break;
            }
        }
    }
    return found;
}

bool CRTConnectionManager::AddUser(CONNECTIONTYPE type, const std::string& uid, CRTConnectionManager::ConnectionInfo* pInfo)
{
    if (uid.length()==0 || !pInfo) return false;
    {
        OSMutexLocker locker(&s_mutexConnection);
        if (s_ConnectionInfoMap.size()==0) {
            s_ConnectionInfoMap.insert(make_pair(uid, pInfo));
            return true;
        }
        CRTConnectionManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            ConnectionLostNotify(uid, it->second->_token);
            delete it->second;
            it->second = NULL;
            s_ConnectionInfoMap.erase(uid);
        }
        s_ConnectionInfoMap.insert(make_pair(uid, pInfo));
    }
    LI("AddUser: show addUser\n");
    ShowConnectionInfo();
    return true;
}

bool CRTConnectionManager::DelUser(CONNECTIONTYPE type, const std::string& uid, std::string& token)
{
    if (uid.length()==0) return false;
    {
        OSMutexLocker locker(&s_mutexConnection);
        if (s_ConnectionInfoMap.size()==0) { return false; }
        CRTConnectionManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
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

void CRTConnectionManager::ConnectionLostNotify(const std::string& uid, const std::string& token)
{
    ModuleInfo* pmi = findModuleInfo(uid, TRANSFERMODULE::mmeeting);
    if (pmi && pmi->pModule) {
        LI("CRTConnectionManager::ConnectionLostNotify uid:%s, token:%s\n", uid.c_str(), token.c_str());
        pmi->pModule->ConnectionLostNotify(uid, token);
    } else {
        LE("pmi->pModule is NULL\n");
    }
    pmi = findModuleInfo(uid, TRANSFERMODULE::mmsgqueue);
    if (pmi && pmi->pModule) {
        LI("CRTConnectionManager::ConnectionLostNotify uid:%s, token:%s\n", uid.c_str(), token.c_str());
        pmi->pModule->ConnectionLostNotify(uid, token);
    } else {
        LE("pmi->pModule is NULL\n");
    }
}

void CRTConnectionManager::ConnectionConnNotify(const std::string& uid, const std::string& token)
{
    ModuleInfo* pmi = findModuleInfo(uid, TRANSFERMODULE::mmeeting);
    if (pmi && pmi->pModule) {
        LI("CRTConnectionManager::ConnectionConnNotify uid:%s, token:%s\n", uid.c_str(), token.c_str());
        pmi->pModule->ConnectionConnNotify(uid, token);
    } else {
        LE("pmi->pModule is NULL\n");
    }
    pmi = findModuleInfo(uid, TRANSFERMODULE::mmsgqueue);
    if (pmi && pmi->pModule) {
        LI("CRTConnectionManager::ConnectionConnNotify uid:%s, token:%s\n", uid.c_str(), token.c_str());
        pmi->pModule->ConnectionConnNotify(uid, token);
    } else {
        LE("pmi->pModule is NULL\n");
    }
}

void CRTConnectionManager::TransferSessionLostNotify(const std::string& sid)
{
    DelModuleInfo(sid);
    DelTypeModuleSession(sid);
    LI("RTConnectionManager::TransferSessionLostNotify sessionid:%s\n", sid.c_str());
}

void CRTConnectionManager::TransferMsg(MSGTYPE mType, long long mseq, const std::string& uid, const std::string& msg)
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

void CRTConnectionManager::ShowConnectionInfo()
{
    return;/*
    if (s_ConnectionInfoMap.size()==0) { return; }
    CRTConnectionManager::ConnectionInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexConnection);
        CRTConnectionManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.begin();
        LI("ShowConnectionInfo size:%d\n", (int)s_ConnectionInfoMap.size());
        for (; it!=s_ConnectionInfoMap.end(); it++) {
            pInfo = it->second;
            LI("userid:%s, uid:%s, token:%s\n", it->first.c_str(), pInfo->_userId.c_str(), pInfo->_token.c_str());
        }
    }*/
}
