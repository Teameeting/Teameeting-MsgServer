//
//  RTConnectionManager.cpp
//  dyncRTConnector
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTConnectionManager.h"
#include <assert.h>
#include "md5.h"
#include "md5digest.h"
#include "OSMutex.h"
#include "RTHiredis.h"
#include <algorithm>

static char          s_curMicroSecStr[32];
static unsigned char s_digest[16];

void RTConnectionManager::GenericSessionId(std::string& strId)
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


RTConnectionManager::ModuleInfo* RTConnectionManager::findModuleInfo(const std::string& userid, TRANSFERMODULE module)
{
    RTConnectionManager::ModuleInfo *pInfo = NULL;
    RTConnectionManager::ModuleInfoMaps* maps = GetModuleInfo();
    RTConnectionManager::ModuleInfoMaps::iterator it = maps->begin();
    for (; it!=maps->end(); it++) {
        if (it->second && it->second->othModuleType == module) {
            pInfo = it->second;
            LI("%s FIND info.moduletype:%d, module:%d\n", __FUNCTION__, pInfo->othModuleType, module);
            break;
        }
    }
    return pInfo;
}

RTConnectionManager::ConnectionInfo* RTConnectionManager::findConnectionInfoById(const std::string& cid)
{
    RTConnectionManager::ConnectionInfo* pInfo = NULL;
    RTConnectionManager::ConnectionInfoMaps* maps = GetConnectionInfo();
    RTConnectionManager::ConnectionInfoMaps::iterator it = maps->find(cid);
    if (it!=maps->end()) {
        pInfo = it->second;
    }
    return pInfo;
}

bool RTConnectionManager::AddModuleInfo(RTConnectionManager::ModuleInfo* pmi, const std::string& sid)
{
    GetModuleInfo()->insert(make_pair(sid, pmi));
    return true;
}

bool RTConnectionManager::DelModuleInfo(const std::string& sid)
{
    ModuleInfoMaps::iterator it = GetModuleInfo()->find(sid);
    if (it!=GetModuleInfo()->end()) {
        GetModuleInfo()->erase(sid);
    }
    return true;
}

bool RTConnectionManager::AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid)
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

bool RTConnectionManager::DelTypeModuleSession(const std::string& sid)
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

bool RTConnectionManager::AddUser(CONNECTIONTYPE type, const std::string& uid, RTConnectionManager::ConnectionInfo* pInfo)
{
    if (uid.length()==0 || !pInfo) return false;
    GetConnectionInfo()->insert(make_pair(uid, pInfo));
    RTHiredisLocal::Instance()->CmdHSet(HI_USER_CONNECTOR_ID, uid, ConnectorId());
    return true;
}

bool RTConnectionManager::DelUser(CONNECTIONTYPE type, const std::string& uid)
{
    if (uid.length()==0) return false;
    std::string res;
    GetConnectionInfo()->erase(uid);
    RTHiredisLocal::Instance()->CmdHDel(HI_USER_CONNECTOR_ID, uid, res);
    return true;
}

void RTConnectionManager::ConnectionLostNotify(const std::string& uid)
{
    ModuleInfo* pmi = findModuleInfo(uid, TRANSFERMODULE::mmeeting);
    if (pmi && pmi->pModule) {
        pmi->pModule->ConnectionLostNotify(uid);
    } else {
        LE("pmi->pModule is NULL\n");
    }
    DelUser(CONNECTIONTYPE::connectiontype_invalid, uid);
}

void RTConnectionManager::TransferSessionLostNotify(const std::string& sid)
{
    DelModuleInfo(sid);
    DelTypeModuleSession(sid);
    LI("RTConnectionManager::TransferSessionLostNotify sessionid:%s\n", sid.c_str());
}