//
//  RTConnectionManager.cpp
//  dyncRTModule
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "LRTConnManager.h"
#include <algorithm>
#include <assert.h>
#include <json/json.h>
#include "OSMutex.h"
#include "LRTTransferSession.h"
#include "LRTGroupSession.h"
#include "RTZKClient.hpp"
#include "ProtoCommon.h"

static int ticket_time = 0;

std::string     LRTConnManager::s_cohttpIp;
unsigned short  LRTConnManager::s_cohttpPort;
std::string     LRTConnManager::s_cohttpHost;

static OSMutex       s_mutex;
static OSMutex       s_mutexModule;
static OSMutex       s_mutexTypeModule;
static OSMutex       s_mutexConnection;

static LRTConnManager::ModuleInfoMaps                 s_ModuleInfoMap(0);
static LRTConnManager::TypeModuleSessionInfoLists     s_TypeModuleSessionInfoList(0);
static LRTConnManager::ConnectionInfoMaps             s_ConnectionInfoMap(0);


LRTConnManager::ModuleInfo* LRTConnManager::findConnModuleInfo(const std::string& userid)
{
    return findModuleInfo(userid, pms::ETransferModule::MCONNECTOR);
}

LRTConnManager::ModuleInfo* LRTConnManager::findModuleInfo(const std::string& uid, pms::ETransferModule module)
{
    LRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        LRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if (it->second && it->second->othModuleType == module) {
                pInfo = it->second;
                break;
            }
        }
    }
    return pInfo;
}

LRTConnManager::ModuleInfo* LRTConnManager::findModuleInfoBySid(const std::string& sid)
{

    LRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        LRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
        if (it!=s_ModuleInfoMap.end()) {
            pInfo = it->second;
        }
    }
    return pInfo;
}

LRTConnManager::ModuleInfo* LRTConnManager::findModuleInfoByMid(const std::string& userid, const std::string& connector)
{

    if (userid.length()==0 || connector.length()==0) {
        LE("findConnectorInfoById userid or connector is 0\n");
        return NULL;
    }
    LRTConnManager::ModuleInfo* pInfo = NULL;
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

LRTConnManager::ConnectionInfo* LRTConnManager::findConnectionInfoById(const std::string& uid)
{
    LRTConnManager::ConnectionInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexConnection);
        LRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            pInfo = it->second;
        }
    }
    return pInfo;
}


bool LRTConnManager::SendToGroupModule(const std::string& userid, const std::string& msg)
{
    LRTConnManager::ConnectionInfo* pci = findConnectionInfoById(userid);
    if (!pci) {
        LE("LRTConnManager::SendToGroupModule not find user:%s connection\n", userid.c_str());
        return false;
    } else { //!pci
        if (pci->_pConn && pci->_pConn->IsLiveSession()) {
            if (pci->_connType == pms::EConnType::TTCP) {
                LRTGroupSession *ct = dynamic_cast<LRTGroupSession*>(pci->_pConn);
                if (ct) {
                    ct->SendTransferData(msg);
                }
            } else {
                LE("LRTConnManager::SendToGroupModule conn type not handle :%d\n", pci->_connType);
            }
            return true;
        } else {
            LE("LRTConnManager::SendToGroupModule conn not alive\n");
             return false;
        }
    }
}

void LRTConnManager::InitManager()
{
    m_logicalAddrList.clear();
    m_connectorAddrList.clear();
    m_dispatcherAddrList.clear();
}

void LRTConnManager::UninManager()
{
    m_logicalAddrList.clear();
    m_connectorAddrList.clear();
    m_dispatcherAddrList.clear();
}


bool LRTConnManager::ConnectLogical()
{
    if (m_logicalAddrList.size() == 0) {
        return false;
    }
    if (m_pConnDispatcher==NULL)
        m_pConnDispatcher = new LRTConnDispatcher();
    std::list<std::string>::iterator it;
    for (it=m_logicalAddrList.begin(); it!=m_logicalAddrList.end(); it++) {
        std::string s = *it;
        printf("ConnectLogical logical addr:%s\n", s.c_str());
        char ip[16] = {0};
        unsigned int port = 0;
        sscanf(s.c_str(), "%s %u", ip, &port);
        printf("ip:%s, port:%u\n", ip, port);
        if (strlen(ip)>0 && port > 2048) {
            DoConnectLogical(ip, port);
        }
    }
    return true;
}

bool LRTConnManager::TryConnectLogical(const std::string ip, unsigned short port)
{
    LI("LRTConnManager::TryConneectLogical ip:%s, port:%u\n", ip.c_str(), port);
    LRTTransferSession* logicalSession = new LRTTransferSession();
    logicalSession->Init();
    // conn to logical

    bool ok = false;
    int times = 0;
    do{
        ok = logicalSession->Connect(ip, port);
        LI("try %d times to connect logical server %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        logicalSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(logicalSession);
        if (m_pConnDispatcher)
            m_pConnDispatcher->Signal(Task::kIdleEvent);
        return false;
    }
}

bool LRTConnManager::ConnectConnector()
{
    if (m_connectorAddrList.size() == 0) {
        return false;
    }
    if (m_pConnDispatcher==NULL)
        m_pConnDispatcher = new LRTConnDispatcher();
    std::list<std::string>::iterator it;
    for (it=m_connectorAddrList.begin(); it!=m_connectorAddrList.end(); it++) {
        std::string s = *it;
        printf("ConnectConnector connector addr:%s\n", s.c_str());
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

bool LRTConnManager::TryConnectConnector(const std::string ip, unsigned short port)
{
    LI("LRTConnManager::TryConneectConnector ip:%s, port:%u\n", ip.c_str(), port);
    LRTTransferSession* connectorSession = new LRTTransferSession();
    connectorSession->Init();
    // conn to connector

    bool ok = false;
    int times = 0;
    do{
        ok = connectorSession->Connect(ip, port);
        LI("try %d times to connect connector server %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        connectorSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(connectorSession);
        if (m_pConnDispatcher)
            m_pConnDispatcher->Signal(Task::kIdleEvent);
        return false;
    }
}

bool LRTConnManager::ConnectDispatcher()
{
    if (m_dispatcherAddrList.size() == 0) {
        return false;
    }
    if (m_pConnDispatcher==NULL)
        m_pConnDispatcher = new LRTConnDispatcher();
    std::list<std::string>::iterator it;
    for (it=m_dispatcherAddrList.begin(); it!=m_dispatcherAddrList.end(); it++) {
        std::string s = *it;
        printf("ConnectDispatcher dispatcher addr:%s\n", s.c_str());
        char ip[16] = {0};
        unsigned int port = 0;
        sscanf(s.c_str(), "%s %u", ip, &port);
        printf("ip:%s, port:%u\n", ip, port);
        if (strlen(ip)>0 && port > 2048) {
            DoConnectDispatcher(ip, port);
        }
    }
    return true;
}

bool LRTConnManager::TryConnectDispatcher(const std::string ip, unsigned short port)
{
    LI("LRTConnManager::TryConneectLogical ip:%s, port:%u\n", ip.c_str(), port);
    LRTTransferSession* dispatcherSession = new LRTTransferSession();
    dispatcherSession->Init();
    // conn to dispatcher

    bool ok = false;
    int times = 0;
    do{
        ok = dispatcherSession->Connect(ip, port);
        LI("try %d times to connect dispatcher server %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        dispatcherSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(dispatcherSession);
        if (m_pConnDispatcher)
            m_pConnDispatcher->Signal(Task::kIdleEvent);
        return false;
    }
}


void LRTConnManager::RefreshConnection()
{
    ModuleInfo* pmi = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            pmi = it->second;
            //if (pmi && pmi->othModuleType == pms::ETransferModule::MCONNECTOR) {
                if (pmi && pmi->pModule && pmi->pModule->RefreshTime()) {
                    pmi->pModule->KeepAlive();
                }
            //}
        }
    }
}

void LRTConnManager::SendTransferData(const std::string mid, const std::string uid, const std::string msg)
{
     if (m_dispatcherSession && m_dispatcherSession->IsLiveSession())
     {
        m_dispatcherSession->SendTransferData(msg);
     } else {
         LE("LRTConnManager::SendTransferData m_dispatcherSession can not use, has error\n");
     }
}

void LRTConnManager::PushNewMsg2Queue(const std::string& str)
{
     if (m_logicalSession && m_logicalSession->IsLiveSession())
     {
         m_logicalSession->PushNewMsg2Queue(str);
     }
}

void LRTConnManager::PushSeqnReq2Queue(const std::string& str)
{
     if (m_logicalSession && m_logicalSession->IsLiveSession())
     {
         m_logicalSession->PushSeqnReq2Queue(str);
     }
}

void LRTConnManager::PushDataReq2Queue(const std::string& str)
{
     if (m_logicalSession && m_logicalSession->IsLiveSession())
     {
         printf("LRTConnManager::PushDataReq2Queue ticket_time:%d\n", ++ticket_time);
         m_logicalSession->PushDataReq2Queue(str);
     }
}

bool LRTConnManager::SignalKill()
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

bool LRTConnManager::ClearAll()
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
    m_logicalAddrList.clear();
     return true;
}

bool LRTConnManager::AddModuleInfo(LRTConnManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    LRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        LRTConnManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool LRTConnManager::DelModuleInfo(const std::string& sid, EventData& data)
{
    OSMutexLocker locker(&s_mutexModule);
    LRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        LRTConnManager::ModuleInfo *p = it->second;
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

bool LRTConnManager::AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid)
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

bool LRTConnManager::DelTypeModuleSession(const std::string& sid)
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

bool LRTConnManager::AddUser(pms::EConnType type, const std::string& uid, LRTConnManager::ConnectionInfo* pInfo)
{
    if (uid.length()==0 || !pInfo) return false;
    {
        OSMutexLocker locker(&s_mutexConnection);
        LRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            //ConnectionLostNotify(uid, it->second->_token);
            delete it->second;
            it->second = NULL;
            s_ConnectionInfoMap.erase(uid);
        }
        s_ConnectionInfoMap.insert(make_pair(uid, pInfo));
    }
    return true;
}

bool LRTConnManager::DelUser(pms::EConnType type, const std::string& uid, std::string& token)
{
    if (uid.length()==0) return false;
    {
        OSMutexLocker locker(&s_mutexConnection);
        LRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
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




void LRTConnManager::TransferSessionLostNotify(const std::string& sid)
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
        RTEventTimer* timer = new RTEventTimer(RETRY_MAX_TIME, &LRTConnManager::DispTimerCallback);
        timer->DataDelay(s.c_str(), (int)s.length());
        LI("Waiting for Session Reconnecting...");
    }
#endif
}

void LRTConnManager::OnTLogin(const std::string& uid, const std::string& token, const std::string& connector)
{

}

void LRTConnManager::OnTLogout(const std::string& uid, const std::string& token, const std::string& connector)
{

}

void LRTConnManager::ProcessRecvEvent(const char*pData, int nLen)
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
        if (data.connect.module == pms::ETransferModule::MCONNECTOR) {// connect to connector
            TryConnectLogical(data.connect.ip, data.connect.port);
        }
    }
}

void LRTConnManager::ProcessTickEvent(const char*pData, int nLen)
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

void LRTConnManager::PostDataStatic(const char* pData, int nLen)
{
    if (m_pConnDispatcher)
        m_pConnDispatcher->PostData(pData, nLen);
}

int LRTConnManager::DispTimerCallback(const char*pData, int nLen)
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
            if (root["module"].asInt()== pms::ETransferModule::MCONNECTOR) {// connect to connector
                s = "/dync/msgserver/connector/" + root["ip"].asString();
            } else {
                return 0;
            }
            if (s.length()>0) {
                if (RTZKClient::Instance().CheckNodeExists(s)) {
                    LRTConnManager::Instance().PostDataStatic(pData, nLen);
                } else {
                    RTEventTimer* timer = new RTEventTimer(RETRY_MAX_TIME, &LRTConnManager::DispTimerCallback);
                    timer->DataDelay(pData, nLen);
                }
            }
        }
    }
    return 0;
}

///////////////////////////////////////////////////////////
////////////////////private////////////////////////////////
///////////////////////////////////////////////////////////

bool LRTConnManager::DoConnectLogical(const std::string ip, unsigned short port)
{
    //LRTTransferSession* logicalSession = new LRTTransferSession();
    m_logicalSession = new LRTTransferSession();
    m_logicalSession->Init();
    // conn to logical
    while (!m_logicalSession->Connect(ip, port)) {
        LI("connecting to logical server %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, port, m_logicalSession->GetSocket()->GetSocketFD());
    m_logicalSession->EstablishConnection();
    return true;
}

bool LRTConnManager::DoConnectConnector(const std::string ip, unsigned short port)
{
    LRTTransferSession* connectorSession = new LRTTransferSession();
    connectorSession->Init();
    // conn to logical
    while (!connectorSession->Connect(ip, port)) {
        LI("connecting to connector server %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, port, connectorSession->GetSocket()->GetSocketFD());
    connectorSession->EstablishConnection();
    return true;
}

bool LRTConnManager::DoConnectDispatcher(const std::string ip, unsigned short port)
{
    m_dispatcherSession = new LRTTransferSession();
    m_dispatcherSession->Init();
    // conn to logical
    while (!m_dispatcherSession->Connect(ip, port)) {
        LI("connecting to dispatcher server %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, port, m_dispatcherSession->GetSocket()->GetSocketFD());
    m_dispatcherSession->EstablishConnection();
    return true;
}
