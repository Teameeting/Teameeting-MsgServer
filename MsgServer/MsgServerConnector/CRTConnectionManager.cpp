//
//  CRTConnectionManager.cpp
//  dyncRTConnector
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTConnectionManager.h"
#include <assert.h>
#include "md5.h"
#include "md5digest.h"
#include "OSMutex.h"
#include <algorithm>

static char          s_curMicroSecStr[32] = {0};
static unsigned char s_digest[16] = {0};

static OSMutex          s_mutex;
static OSMutex          s_mutexModule;
static OSMutex          s_mutexConnection;
static OSMutex          s_mutexTypeModule;

static CRTConnectionManager::ModuleInfoMaps               s_ModuleInfoMap(0);
static CRTConnectionManager::ConnectionInfoMaps           s_ConnectionInfoMap(0);
static CRTConnectionManager::TypeModuleSessionInfoLists   s_TypeModuleSessionInfoList(0);
static CRTConnectionManager::UserSessionInfoLists         s_UserSessionInfoList(0);
static CRTConnectionManager::UserSessionInfoMaps          s_UserSessionInfoMap(0);


void CRTConnectionManager::GenericSessionId(std::string& strId)
{
    OSMutexLocker locker(&s_mutex);
    SInt64 curTime = OS::Microseconds();
    MD5_CTX context;
    StrPtrLen hashStr;
    memset(s_curMicroSecStr, 0, 128);
    memset(s_digest, 0, 16);
    qtss_sprintf(s_curMicroSecStr, "%lld", curTime);
    MD5_Init(&context);
    MD5_Update(&context, (unsigned char*)s_curMicroSecStr, (unsigned int)strlen((const char*)s_curMicroSecStr));
    MD5_Update(&context, (unsigned char*)m_lastUpdateTime.c_str(), (unsigned int)m_lastUpdateTime.length());
    MD5_Final(s_digest, &context);
    HashToString(s_digest, &hashStr);
    char* p = hashStr.GetAsCString();
    strId = p;
    delete p;
    p = NULL;
    hashStr.Delete();
    LI("GenericSessionId:%s\n", strId.c_str());
    m_lastUpdateTime = s_curMicroSecStr;
}


CRTConnectionManager::ModuleInfo* CRTConnectionManager::findModuleInfo(const std::string& userid, TRANSFERMODULE module)
{
    OSMutexLocker locker(&s_mutexModule);
    if (s_ModuleInfoMap.size()==0) { return NULL; }
    CRTConnectionManager::ModuleInfo *pInfo = NULL;
    CRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
    LI("=====findModuleInfo module:%d, maps.size:%d, user:%s\n", (int)module, (int)s_ModuleInfoMap.size(), userid.c_str());
    for (; it!=s_ModuleInfoMap.end(); it++) {
        if (it->second && it->second->othModuleType == module) {
            pInfo = it->second;
            LI("%s FIND info.moduletype:%d, module:%d\n", __FUNCTION__, pInfo->othModuleType, module);
            break;
        }
    }
    return pInfo;
}

CRTConnectionManager::ConnectionInfo* CRTConnectionManager::findConnectionInfoById(const std::string& uid)
{
    OSMutexLocker locker(&s_mutexConnection);
    if (s_ConnectionInfoMap.size()==0) { return NULL; }
    CRTConnectionManager::ConnectionInfo* pInfo = NULL;
    LI("findConnection show find uid:%s\n", uid.c_str());
    ShowConnectionInfo();
    CRTConnectionManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
    if (it!=s_ConnectionInfoMap.end()) {
        pInfo = it->second;
    } else {
        LI("findConnection after show NOOOOOOT find uid:%s\n", uid.c_str());
    }
    return pInfo;
}

bool CRTConnectionManager::AddModuleInfo(CRTConnectionManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    LI("AddModuleInfo 11 getpid:%lld, pthread_self:%llu, module:%p, size:%d, sid:%s\n", getpid(), pthread_self(), &s_ModuleInfoMap, (int)s_ModuleInfoMap.size(), sid.c_str());
    DelModuleInfo(sid);
    LI("AddModuleInfo 22 getpid:%lld, pthread_self:%llu, module:%p, size:%d, sid:%s\n", getpid(), pthread_self(), &s_ModuleInfoMap, (int)s_ModuleInfoMap.size(), sid.c_str());
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    LI("AddModuleInfo 33 getpid:%lld, pthread_self:%llu, module:%p, size:%d, sid:%s\n", getpid(), pthread_self(), &s_ModuleInfoMap, (int)s_ModuleInfoMap.size(), sid.c_str());
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
    OSMutexLocker locker(&s_mutexTypeModule);
    TypeModuleSessionInfo* pInfo = NULL;
    if (s_TypeModuleSessionInfoList.size()==0) {
        pInfo = new TypeModuleSessionInfo();
        pInfo->moduleType = mtype;
        pInfo->moduleId = mid;
        pInfo->sessionIds.push_front(sid);
        s_TypeModuleSessionInfoList.push_front(pInfo);
        return true;
    }
    bool found = false;
    TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
    LI("AddTypeModuleSession  size:%d, type:%d, mid:%s, sid:%s\n", (int)s_TypeModuleSessionInfoList.size(), (int)mtype, mid.c_str(), sid.c_str());
    for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
        if ((*it) && (*it)->moduleId.compare(mid) == 0) {
            pInfo = *it;
            found = true;
    LI("AddTypeModuleSession find module size:%d, type:%d, mid:%s, sid:%s\n", (int)s_TypeModuleSessionInfoList.size(), (int)mtype, mid.c_str(), sid.c_str());
            break;
        }
    }
    if (found) {
    LI("AddTypeModuleSession 11111111111  infoList.size:%d, find type:%d, mid:%s, sid:%s\n", (int)s_TypeModuleSessionInfoList.size(), (int)mtype, mid.c_str(), sid.c_str());
        pInfo->sessionIds.push_front(sid);
    } else {
        pInfo = new TypeModuleSessionInfo();
        pInfo->moduleType = mtype;
        pInfo->moduleId = mid;
        pInfo->sessionIds.push_front(sid);
        s_TypeModuleSessionInfoList.push_front(pInfo);
    LI("AddTypeModuleSession 22222222222 not find  infoList.size:%d, type:%d, mid:%s, sid:%s\n", (int)s_TypeModuleSessionInfoList.size(), (int)mtype, mid.c_str(), sid.c_str());
    }
    return true;
}

bool CRTConnectionManager::DelTypeModuleSession(const std::string& sid)
{
    OSMutexLocker locker(&s_mutexTypeModule);
    if (s_TypeModuleSessionInfoList.size()==0) { return false; }
    bool found = false;
    TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
    TypeModuleSessionInfo* pInfo = NULL;
    for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
        pInfo = *it;
        std::list<std::string>::iterator sit = std::find(pInfo->sessionIds.begin(), pInfo->sessionIds.end(), sid);
        if (sit!=pInfo->sessionIds.end()) {
            pInfo->sessionIds.erase(sit);
            found = true;
        }

    }
    return found;
}

bool CRTConnectionManager::AddUser(CONNECTIONTYPE type, const std::string& uid, CRTConnectionManager::ConnectionInfo* pInfo)
{
    OSMutexLocker locker(&s_mutexConnection);
    if (s_ConnectionInfoMap.size()==0) {
        s_ConnectionInfoMap.insert(make_pair(uid, pInfo));
        LI("AddUser: show addUser\n");
        ShowConnectionInfo();
        return true;
    }
    if (uid.length()==0 || !pInfo) return false;
    std::string token;
    if(DelUser(type, uid, token)) {
        ConnectionLostNotify(uid, token);
    }
    s_ConnectionInfoMap.insert(make_pair(uid, pInfo));
    LI("AddUser: show addUser\n");
    ShowConnectionInfo();
    return true;
}

bool CRTConnectionManager::DelUser(CONNECTIONTYPE type, const std::string& uid, std::string& token)
{
    OSMutexLocker locker(&s_mutexConnection);
    if (s_ConnectionInfoMap.size()==0) { return false; }
    if (uid.length()==0) return false;
    CRTConnectionManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
    LI("DelUser: show: delUser\n");
    ShowConnectionInfo();
    if (it!=s_ConnectionInfoMap.end()) {
        CRTConnectionManager::ConnectionInfo* p = it->second;
        token = p->_token;
        delete p;
        p = NULL;
        s_ConnectionInfoMap.erase(uid);
        return true;
    } else {
        return false;
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
}

void CRTConnectionManager::TransferSessionLostNotify(const std::string& sid)
{
    DelModuleInfo(sid);
    DelTypeModuleSession(sid);
    LI("RTConnectionManager::TransferSessionLostNotify sessionid:%s\n", sid.c_str());
}

void CRTConnectionManager::ShowConnectionInfo()
{
    OSMutexLocker locker(&s_mutexConnection);
    if (s_ConnectionInfoMap.size()==0) { return ; }
    CRTConnectionManager::ConnectionInfo* pInfo = NULL;
    CRTConnectionManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.begin();
    LI("ShowConnectionInfo size:%d\n", (int)s_ConnectionInfoMap.size());
    for (; it!=s_ConnectionInfoMap.end(); it++) {
        pInfo = it->second;
        LI("userid:%s, uid:%s, token:%s\n", it->first.c_str(), pInfo->_userId.c_str(), pInfo->_token.c_str());
    }
}
