//
//  MSMsgUtil.m
//  SdkMsgClient::os
//
//  Created by hp on 7/12/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "MSMsgUtil.h"
#include "MsgClient.h"
#include "MSMessage.h"
#include "MSSubMessage.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"


MSMessage*  MSMsgUtil::EncodeMessageWithTxtMsg(MSTxtMessage* txtMsg, int msgType)
{
    MSMessage* mMsg = new MSMessage();
    mMsg->SetContent(txtMsg->GetContent());
    mMsg->SetGroupId(txtMsg->GetGroupId());
    mMsg->SetPush(txtMsg->GetPush());
    mMsg->SetToId(txtMsg->GetToId());
    mMsg->SetNickName(MsgClient::Instance().MCGetNickName());
    mMsg->SetUiconUrl(MsgClient::Instance().MCGetUIconUrl());
    mMsg->SetFromId(MsgClient::Instance().MCGetUserId());
    mMsg->SetToNickName(txtMsg->GetToNickName());
    mMsg->SetMsgType((MCMsgType)msgType);

    return mMsg;
}

MSMessage*  MSMsgUtil::EncodeMessageWithLivMsg(MSLivMessage* livMsg, int msgType)
{
    MSMessage* mMsg = new MSMessage();
    mMsg->SetGroupId(livMsg->GetGroupId());
    mMsg->SetToId(livMsg->GetToId());
    mMsg->SetPush(livMsg->GetPush());
    mMsg->SetNickName(MsgClient::Instance().MCGetNickName());
    mMsg->SetUiconUrl(MsgClient::Instance().MCGetUIconUrl());
    mMsg->SetFromId(MsgClient::Instance().MCGetUserId());
    mMsg->SetToNickName(livMsg->GetToNickName());
    mMsg->SetFlag(livMsg->GetFlag());
    mMsg->SetMsgType((MCMsgType)msgType);

    return mMsg;
}

MSMessage*  MSMsgUtil::EncodeMessageWithRenMsg(MSRenMessage* renMsg, int msgType)
{
    MSMessage* mMsg = new MSMessage();
    mMsg->SetGroupId(renMsg->GetGroupId());
    mMsg->SetToId(renMsg->GetToId());
    mMsg->SetPush(renMsg->GetPush());
    mMsg->SetNickName(MsgClient::Instance().MCGetNickName());
    mMsg->SetUiconUrl(MsgClient::Instance().MCGetUIconUrl());
    mMsg->SetFromId(MsgClient::Instance().MCGetUserId());
    mMsg->SetCash(renMsg->GetCash());
    mMsg->SetWishcont(renMsg->GetWishcont());
    mMsg->SetToNickName(renMsg->GetToNickName());
    mMsg->SetMsgType((MCMsgType)msgType);

    return mMsg;
}

MSMessage*  MSMsgUtil::EncodeMessageWithBlkMsg(MSBlkMessage* blkMsg, int msgType)
{
    MSMessage* mMsg = new MSMessage();
    mMsg->SetGroupId(blkMsg->GetGroupId());
    mMsg->SetToId(blkMsg->GetToId());
    mMsg->SetPush(blkMsg->GetPush());
    mMsg->SetNickName(MsgClient::Instance().MCGetNickName());
    mMsg->SetUiconUrl(MsgClient::Instance().MCGetUIconUrl());
    mMsg->SetFromId(MsgClient::Instance().MCGetUserId());
    mMsg->SetToNickName(blkMsg->GetToNickName());
    mMsg->SetFlag(blkMsg->GetFlag());
    mMsg->SetMsgType((MCMsgType)msgType);

    return mMsg;
}

MSMessage*  MSMsgUtil::EncodeMessageWithFbdMsg(MSFbdMessage* fbdMsg, int msgType)
{
    MSMessage* mMsg = new MSMessage();
    mMsg->SetGroupId(fbdMsg->GetGroupId());
    mMsg->SetToId(fbdMsg->GetToId());
    mMsg->SetPush(fbdMsg->GetPush());
    mMsg->SetNickName(MsgClient::Instance().MCGetNickName());
    mMsg->SetUiconUrl(MsgClient::Instance().MCGetUIconUrl());
    mMsg->SetFromId(MsgClient::Instance().MCGetUserId());
    mMsg->SetToNickName(fbdMsg->GetToNickName());
    mMsg->SetFlag(fbdMsg->GetFlag());
    mMsg->SetMsgType((MCMsgType)msgType);

    return mMsg;
}

MSMessage*  MSMsgUtil::EncodeMessageWithMgrMsg(MSMgrMessage* mgrMsg, int msgType)
{
    MSMessage* mMsg = new MSMessage();
    mMsg->SetGroupId(mgrMsg->GetGroupId());
    mMsg->SetToId(mgrMsg->GetToId());
    mMsg->SetPush(mgrMsg->GetPush());
    mMsg->SetNickName(MsgClient::Instance().MCGetNickName());
    mMsg->SetUiconUrl(MsgClient::Instance().MCGetUIconUrl());
    mMsg->SetFromId(MsgClient::Instance().MCGetUserId());
    mMsg->SetToNickName(mgrMsg->GetToNickName());
    mMsg->SetFlag(mgrMsg->GetFlag());
    mMsg->SetMsgType((MCMsgType)msgType);

    return mMsg;
}

MSMessage*  MSMsgUtil::JsonToMessage(const std::string& jsonMsg)
{
    if (jsonMsg.length()==0) return nullptr;
    std::string     content("");
    std::string     extra("");
    std::string     msgId("");
    std::string     toId("");
    std::string     fromId("");
    std::string     groupId("");
    std::string     nickName("");
    std::string     uiconUrl("");
    std::string     cash("");
    std::string     wishcont("");
    std::string     toNickName("");
    unsigned int    millSec=0;
    MCMsgType       msgType=MCMsgTypeTtxt;
    int             flag=0;
    int             push=0;

    rapidjson::Document jsonMsgDoc;
    if (!jsonMsgDoc.ParseInsitu<0>((char*)jsonMsg.c_str()).HasParseError())
    {
        if (jsonMsgDoc.HasMember("content")&&jsonMsgDoc["content"].IsString())
        {
            content = jsonMsgDoc["content"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("extra")&&jsonMsgDoc["extra"].IsString())
        {
            extra = jsonMsgDoc["extra"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("msgId")&&jsonMsgDoc["msgId"].IsString())
        {
            msgId = jsonMsgDoc["msgId"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("toId")&&jsonMsgDoc["toId"].IsString())
        {
            toId = jsonMsgDoc["toId"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("fromId")&&jsonMsgDoc["fromId"].IsString())
        {
            fromId = jsonMsgDoc["fromId"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("groupId")&&jsonMsgDoc["groupId"].IsString())
        {
            groupId = jsonMsgDoc["groupId"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("nickName")&&jsonMsgDoc["nickName"].IsString())
        {
            nickName = jsonMsgDoc["nickName"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("uiconUrl")&&jsonMsgDoc["uiconUrl"].IsString())
        {
            uiconUrl = jsonMsgDoc["uiconUrl"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("cash")&&jsonMsgDoc["cash"].IsString())
        {
            cash = jsonMsgDoc["cash"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("wishcont")&&jsonMsgDoc["wishcont"].IsString())
        {
            wishcont = jsonMsgDoc["wishcont"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("toNickName")&&jsonMsgDoc["toNickName"].IsString())
        {
            toNickName = jsonMsgDoc["toNickName"].GetString();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("millSec")&&jsonMsgDoc["millSec"].IsInt())
        {
            millSec = jsonMsgDoc["millSec"].GetInt();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("msgType")&&jsonMsgDoc["msgType"].IsInt())
        {
            msgType = (MCMsgType)jsonMsgDoc["msgType"].GetInt();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("flag")&&jsonMsgDoc["flag"].IsInt())
        {
            flag = jsonMsgDoc["flag"].GetInt();
        } else {
            return nullptr;
        }
        if (jsonMsgDoc.HasMember("push")&&jsonMsgDoc["push"].IsInt())
        {
            push = jsonMsgDoc["push"].GetInt();
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
    MSMessage *msg = new MSMessage();
    msg->SetContent(content);
    msg->SetExtra(extra);
    msg->SetMsgId(msgId);
    msg->SetToId(toId);
    msg->SetFromId(fromId);
    msg->SetGroupId(groupId);
    msg->SetNickName(nickName);
    msg->SetUiconUrl(uiconUrl);
    msg->SetCash(cash);
    msg->SetWishcont(wishcont);
    msg->SetToNickName(toNickName);
    msg->SetMillSec(millSec);
    msg->SetMsgType((MCMsgType)msgType);
    msg->SetFlag(flag);
    msg->SetPush(push);
    return msg;
}

std::string*    MSMsgUtil::MessageToJson(MSMessage& msg)
{
    rapidjson::Document jDoc;
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    jDoc.SetObject();
    jDoc.AddMember("content", msg.GetContent().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("extra", msg.GetExtra().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("msgId", msg.GetMsgId().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("toId", msg.GetToId().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("fromId", msg.GetFromId().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("groupId", msg.GetGroupId().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("nickName", msg.GetNickName().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("uiconUrl", msg.GetUiconUrl().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("cash", msg.GetCash().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("wishcont", msg.GetWishcont().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("toNickName", msg.GetToNickName().c_str(), jDoc.GetAllocator());
    jDoc.AddMember("millSec", msg.GetMillSec(), jDoc.GetAllocator());
    jDoc.AddMember("msgType", msg.GetMsgType(), jDoc.GetAllocator());
    jDoc.AddMember("flag", msg.GetFlag(), jDoc.GetAllocator());
    jDoc.AddMember("push", msg.GetPush(), jDoc.GetAllocator());

    jDoc.Accept(writer);
    return new std::string(sb.GetString());
}
