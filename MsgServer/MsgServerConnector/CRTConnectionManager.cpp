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
#include "RTHiredis.h"
#include <algorithm>

static char          s_curMicroSecStr[32];
static unsigned char s_digest[16];

void CRTConnectionManager::GenericSessionId(std::string& strId)
{
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
    strId = hashStr.GetAsCString();
    m_lastUpdateTime = s_curMicroSecStr;
}


CRTConnectionManager::ModuleInfo* CRTConnectionManager::findModuleInfo(const std::string& userid, TRANSFERMODULE module)
{
    CRTConnectionManager::ModuleInfo *pInfo = NULL;
    CRTConnectionManager::ModuleInfoMaps* maps = GetModuleInfo();
    CRTConnectionManager::ModuleInfoMaps::iterator it = maps->begin();
    for (; it!=maps->end(); it++) {
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
    CRTConnectionManager::ConnectionInfo* pInfo = NULL;
    CRTConnectionManager::ConnectionInfoMaps* maps = GetConnectionInfo();
    LI("findConnection show find uid:%s\n", uid.c_str());
    ShowConnectionInfo();
    CRTConnectionManager::ConnectionInfoMaps::iterator it = maps->find(uid);
    if (it!=maps->end()) {
        pInfo = it->second;
    } else {
        LI("findConnection after show NOOOOOOT find uid:%s\n", uid.c_str());
    }
    return pInfo;
}

bool CRTConnectionManager::AddModuleInfo(CRTConnectionManager::ModuleInfo* pmi, const std::string& sid)
{
    DelModuleInfo(sid);
    GetModuleInfo()->insert(make_pair(sid, pmi));
    return true;
}

bool CRTConnectionManager::DelModuleInfo(const std::string& sid)
{
    CRTConnectionManager::ModuleInfoMaps* maps = GetModuleInfo();
    CRTConnectionManager::ModuleInfoMaps::iterator it = maps->find(sid);
    if (it!=maps->end()) {
        CRTConnectionManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        maps->erase(sid);
    }
    return true;
}

bool CRTConnectionManager::AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid)
{
    bool found = false;
    TypeModuleSessionInfoLists* infoList = GetTypeModuleSessionInfo();
    TypeModuleSessionInfoLists::iterator it = infoList->begin();
    TypeModuleSessionInfo* pInfo = NULL;
    for (; it!=infoList->end(); it++) {
        if ((*it) && (*it)->moduleId.compare(mid) == 0) {
            pInfo = *it;
            found = true;
            break;
        }
    }
    if (found) {
        pInfo->sessionIds.push_front(sid);
    } else {
        pInfo = new TypeModuleSessionInfo();
        pInfo->moduleType = mtype;
        pInfo->moduleId = mid;
        pInfo->sessionIds.push_front(sid);
        infoList->push_front(pInfo);
    }
    return true;
}

bool CRTConnectionManager::DelTypeModuleSession(const std::string& sid)
{
    bool found = false;
    TypeModuleSessionInfoLists* infoList = GetTypeModuleSessionInfo();
    TypeModuleSessionInfoLists::iterator it = infoList->begin();
    TypeModuleSessionInfo* pInfo = NULL;
    for (; it!=infoList->end(); it++) {
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
    if (uid.length()==0 || !pInfo) return false;
    std::string token;
    if(DelUser(type, uid, token)) {
        ConnectionLostNotify(uid, token);
    }
    GetConnectionInfo()->insert(make_pair(uid, pInfo));
    LI("AddUser: show addUser\n");
    ShowConnectionInfo();
    return true;
}

bool CRTConnectionManager::DelUser(CONNECTIONTYPE type, const std::string& uid, std::string& token)
{
    if (uid.length()==0) return false;
    CRTConnectionManager::ConnectionInfoMaps* maps = GetConnectionInfo();
    CRTConnectionManager::ConnectionInfoMaps::iterator it = maps->find(uid);
    LI("DelUser: show: delUser\n");
    ShowConnectionInfo();
    if (it!=maps->end()) {
        CRTConnectionManager::ConnectionInfo* p = it->second;
        token = p->_token;
        delete p;
        p = NULL;
        maps->erase(uid);
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
    CRTConnectionManager::ConnectionInfo* pInfo = NULL;
    CRTConnectionManager::ConnectionInfoMaps* maps = GetConnectionInfo();
    CRTConnectionManager::ConnectionInfoMaps::iterator it = maps->begin();
    LI("ShowConnectionInfo size:%d\n", (int)maps->size());
    for (; it!=maps->end(); it++) {
        pInfo = it->second;
        LI("userid:%s, uid:%s, token:%s\n", it->first.c_str(), pInfo->_userId.c_str(), pInfo->_token.c_str());
    }
}