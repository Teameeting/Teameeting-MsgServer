//
//  MSMessageManager.m
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "MSMessageManager.h"
#include "MsgClient.h"
#include "rtklog.h"

void MSMessageManager::AddDelegate(const MSSubMessageDelegate* subDelegate)
{
    MsgClient::Instance().MCSetTxtMsgDelegate(subDelegate);
}

void MSMessageManager::DelDelegate(const MSSubMessageDelegate* subDelegate)
{
    MsgClient::Instance().MCSetTxtMsgDelegate(nullptr);
}

void MSMessageManager::SyncMsg()
{
    MsgClient::Instance().MCSyncMsg();
}

void MSMessageManager::Sync2Db()
{
    MsgClient::Instance().MCSync2Db();
}

const char* MSMessageManager::SendTxtMsg(MSTxtMessage* txtMsg)
{
    if (nullptr==txtMsg)
    {
        LI("sendTxtMsg txtMsg is null\n");
        return nullptr;
    }
    LI("send grpid:%s, content:%s\n", txtMsg->GetGroupId().c_str(), txtMsg->GetContent().c_str());
    if ((0==txtMsg->GetGroupId().length()) || 0==txtMsg->GetContent().length())
    {
        LI("sendTxtMsg params is null\n");
        return nullptr;
    }
    std::string msgid("");
    MsgClient::Instance().MCSendTxtMsg(msgid, txtMsg);
    return strdup(msgid.c_str());
}

const char* MSMessageManager::SendTxtMsgTos(MSTxtMessage* txtMsg, std::vector<std::string> users)
{
    if (nullptr==txtMsg) return nullptr;
    if (0==txtMsg->GetGroupId().length() || 0==txtMsg->GetContent().length() || 0==users.size()) return nullptr;
    std::string msgid("");
    std::vector<std::string> vusers;
    for (std::string name : users) {
        vusers.push_back(name);
    }
    MsgClient::Instance().MCSendTxtMsgTos(msgid, txtMsg, vusers);
    return strdup(msgid.c_str());
}

const char* MSMessageManager::SendTxtMsgToUser(MSTxtMessage* txtMsg)
{
    if (nullptr==txtMsg) return nullptr;
    if ((0==txtMsg->GetToId().length())||(txtMsg->GetContent().length()==0)) return nullptr;
    std::string msgid("");
    MsgClient::Instance().MCSendTxtMsgToUsr(msgid, txtMsg);
    return strdup(msgid.c_str());
}

const char* MSMessageManager::SendTxtMsgToUsers(MSTxtMessage* txtMsg, std::vector<std::string> users)
{
    if (nullptr==txtMsg) return nullptr;
    if ((0==txtMsg->GetContent().length())||(0==users.size())) return nullptr;
    std::string msgid("");
    std::vector<std::string> vusers;
    for (std::string name : users) {
        vusers.push_back(name);
    }
    MsgClient::Instance().MCSendTxtMsgToUsrs(msgid, txtMsg, vusers);
    return strdup(msgid.c_str());
}

const char* MSMessageManager::SendNotifyLive(MSLivMessage* livMsg)
{
    if (nullptr==livMsg) return nullptr;
    if ((0==livMsg->GetGroupId().length())) return nullptr;
    std::string msgid("");
    MsgClient::Instance().MCNotifyLive(msgid, livMsg);
    return strdup(msgid.c_str());
}

const char* MSMessageManager::SendNotifyRenEnvelope(MSRenMessage* renMsg)
{
    if (nullptr==renMsg) return nullptr;
    if ((0==renMsg->GetGroupId().length())
            ||(0==renMsg->GetToId().length())
            ||(0==renMsg->GetCash().length())
            ||(0==renMsg->GetWishcont().length())) return nullptr;
    std::string msgid("");
    MsgClient::Instance().MCNotifyRedEnvelope(msgid, renMsg);
    return strdup(msgid.c_str());
}

const char* MSMessageManager::SendNotifyBlacklist(MSBlkMessage* blkMsg, std::vector<std::string> notifys)
{
    if (nullptr==blkMsg) return nullptr;
    if ((0==blkMsg->GetGroupId().length())
            ||(0==blkMsg->GetToId().length())) return nullptr;
    std::string msgid("");
    std::vector<std::string> vusers;
    if (notifys.size()>0) {
        for (std::string name : notifys) {
            vusers.push_back(name);
        }
    }
    MsgClient::Instance().MCNotifyBlacklist(msgid, blkMsg, vusers);
    return strdup(msgid.c_str());
}

const char* MSMessageManager::SendNotifyForbidden(MSFbdMessage* fbdMsg, std::vector<std::string> notifys)
{
    if (nullptr==fbdMsg) return nullptr;
    if ((0==fbdMsg->GetGroupId().length())
        ||(0==fbdMsg->GetToId().length())) return nullptr;
    std::string msgid("");
    std::vector<std::string> vusers;
    if (notifys.size()>0) {
        for (std::string name : notifys) {
            vusers.push_back(name);
        }
    }
    MsgClient::Instance().MCNotifyForbidden(msgid, fbdMsg, vusers);
    return strdup(msgid.c_str());
}

const char* MSMessageManager::SendNotifySettedMgr(MSMgrMessage* mgrMsg)
{
    if (nullptr==mgrMsg) return nullptr;
    if ((0==mgrMsg->GetGroupId().length())||(0==mgrMsg->GetToId().length())) return nullptr;
    std::string msgid("");
    std::vector<std::string> vusers;
    MsgClient::Instance().MCNotifySettedMgr(msgid, mgrMsg, vusers);
    return strdup(msgid.c_str());
}
