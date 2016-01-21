//
//  MRTConnectionManager.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "MRTConnectionManager.h"
#include <assert.h>
#include "md5.h"
#include "md5digest.h"
#include "OSMutex.h"
#include "MRTRoomManager.h"
#include "MRTTransferSession.h"

static char          s_curMicroSecStr[32] = {0};
static unsigned char s_digest[16] = {0};
static OSMutex       s_mutex;
static OSMutex       s_mutexModule;
static OSMutex       s_mutexTypeModule;

static MRTConnectionManager::ModuleInfoMaps                 s_ModuleInfoMap(0);
static MRTConnectionManager::TypeModuleSessionInfoLists     s_TypeModuleSessionInfoList(0);
static MRTConnectionManager::UserSessionInfoLists           s_UserSessionInfoList(0);
static MRTConnectionManager::UserSessionInfoMaps            s_UserSessionInfoMap(0);


void MRTConnectionManager::GenericSessionId(std::string& strId)
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
    m_lastUpdateTime = s_curMicroSecStr;
}


MRTConnectionManager::ModuleInfo* MRTConnectionManager::findModuleInfo(const std::string& userid, TRANSFERMODULE module)
{
    OSMutexLocker locker(&s_mutexModule);
    if (s_ModuleInfoMap.size()==0) { return NULL; }
    MRTConnectionManager::ModuleInfo* pInfo = NULL;
    MRTConnectionManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
    for (; it!=s_ModuleInfoMap.end(); it++) {
        if ((it->second) && (it->second)->othModuleType == module) {
            pInfo = it->second;
            break;
        }
    }
    return pInfo;
}

bool MRTConnectionManager::ConnectConnector()
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

bool MRTConnectionManager::DoConnectConnector(const std::string ip, unsigned short port)
{
    MRTTransferSession* connectorSession = new MRTTransferSession();
    connectorSession->Init();
    // conn to connector

    while(!connectorSession->Connect(ip, port)) {
        LI("connecting to connector server %s:%u, waiting...\n", ip.c_str(), port);
        sleep(1);
    }
    connectorSession->EstablishConnection();
    return true;
}

void MRTConnectionManager::RefreshConnection()
{
    OSMutexLocker locker(&s_mutexModule);
    if (s_ModuleInfoMap.size()==0) { return; }
    ModuleInfo* pmi = NULL;
    ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
    for (; it!=s_ModuleInfoMap.end(); it++) {
        pmi = it->second;
        if (pmi && pmi->othModuleType == TRANSFERMODULE::mconnector) {
            if (pmi->pModule->RefreshTime()) {
                pmi->pModule->KeepAlive();
            }
        }
    }
}


bool MRTConnectionManager::AddModuleInfo(MRTConnectionManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    DelModuleInfo(sid);
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool MRTConnectionManager::DelModuleInfo(const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    if (s_ModuleInfoMap.size()==0) { return true; }
    ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        MRTConnectionManager::ModuleInfo *p = it->second;
        if (p && p->othModuleType == TRANSFERMODULE::mmsgqueue) {
            LE("RTConnectionManager::DelModuleInfo sid:%s\n", sid.c_str());
            MRTRoomManager::Instance()->ClearMsgQueueSession(sid);
        }
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    return true;
}

bool MRTConnectionManager::AddTypeModuleSession(TRANSFERMODULE mtype, const std::string& mid, const std::string& sid)
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
    for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
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
        s_TypeModuleSessionInfoList.push_front(pInfo);
    }
    return true;
}

bool MRTConnectionManager::DelTypeModuleSession(const std::string& sid)
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

void MRTConnectionManager::TransferSessionLostNotify(const std::string& sid)
{
    DelModuleInfo(sid);
    DelTypeModuleSession(sid);
    LI("RTConnectionManager::TransferSessionLostNotify sessionid:%s\n", sid.c_str());
}
