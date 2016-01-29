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


void CRTConnectionManager::GenericSessionId(std::string& strId)
{
    
    SInt64 curTime = 0;
    char* p = NULL;
    MD5_CTX context;
    StrPtrLen hashStr;
    
    char          s_curMicroSecStr[32] = {0};
    unsigned char s_digest[16] = {0};
    memset(s_curMicroSecStr, 0, 128);
    memset(s_digest, 0, 16);
    {
        curTime = OS::Milliseconds();
        qtss_sprintf(s_curMicroSecStr, "%lld", curTime);
        MD5_Init(&context);
        MD5_Update(&context, (unsigned char*)s_curMicroSecStr, (unsigned int)strlen((const char*)s_curMicroSecStr));
        MD5_Final(s_digest, &context);
        HashToString(s_digest, &hashStr);
        p = hashStr.GetAsCString();
        strId.assign(p);
        delete p;
        p = NULL;
        hashStr.Delete();
    }
    //LI("GenericSessionId:%s\n", strId.c_str());
}


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
    ShowConnectionInfo();
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
}

void CRTConnectionManager::TransferSessionLostNotify(const std::string& sid)
{
    DelModuleInfo(sid);
    DelTypeModuleSession(sid);
    LI("RTConnectionManager::TransferSessionLostNotify sessionid:%s\n", sid.c_str());
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

bool CRTConnectionManager::ConnectHttpSvrConn()
{
    if (!m_pcoHttpSvrConn) {
        LI("CRTConnectionManager::ConnectHttpSvrConn ok\n");
        m_pcoHttpSvrConn = new rtc::RefCountedObject<CORTHttpSvrConn>();
        m_pcoHttpSvrConn->SetHttpHost(s_cohttpIp, s_cohttpPort, s_cohttpHost);
    } else {
        LI("CRTConnectionManager::ConnectHttpSvrConn error\n");
        return false;
    }
    
    return true;
}

void CRTConnectionManager::PushMeetingMsg(const std::string& sign, const std::string& meetingid, const std::string& pushMsg, const std::string& notification)
{
    if (m_pcoHttpSvrConn && sign.length()>0 && meetingid.length()>0 && pushMsg.length()>0 && notification.length()>0) {
        m_pcoHttpSvrConn->HttpPushMeetingMsg(sign.c_str(), meetingid.c_str(), pushMsg.c_str(), notification.c_str());
    } else {
        LE("CRTConnectionManager::PushMeetingMsg error\n");
    }
}
void CRTConnectionManager::PushCommonMsg(const std::string& sign, const std::string& targetid, const std::string& pushMsg, const std::string& notification)
{
    if (m_pcoHttpSvrConn && sign.length()>0 && targetid.length()>0 && pushMsg.length()>0 && notification.length()>0) {
        m_pcoHttpSvrConn->HttpPushCommonMsg(sign.c_str(), targetid.c_str(), pushMsg.c_str(), notification.c_str());
    }
}

