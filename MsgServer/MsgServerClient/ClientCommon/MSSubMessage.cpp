//
//  MSTxtMessage.mm
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "MSSubMessage.h"

//#pragma mark - MSTxtMessage

MSTxtMessage::MSTxtMessage()
: m_message(new MSMessage())
{
}

MSTxtMessage::~MSTxtMessage()
{
    if (m_message)
    {
        delete m_message;
        m_message = nullptr;
    }
}

int MSTxtMessage::GetPush()
{
    return m_message->GetPush();
}

void MSTxtMessage::SetPush(int push)
{
    m_message->SetPush(push);
}

std::string& MSTxtMessage::GetContent()
{
    return m_message->GetContent();
}

void MSTxtMessage::SetContent(const std::string& content)
{
    m_message->SetContent(content);
}

std::string& MSTxtMessage::GetGroupId()
{
    return m_message->GetGroupId();
}

void MSTxtMessage::SetGroupId(const std::string& groupId)
{
    m_message->SetGroupId(groupId);
}

std::string& MSTxtMessage::GetToId()
{
    return m_message->GetToId();
}

void MSTxtMessage::SetToId(const std::string& toId)
{
    m_message->SetToId(toId);
}

std::string& MSTxtMessage::GetToNickName()
{
    return m_message->GetToNickName();
}

void MSTxtMessage::SetToNickName(const std::string& toNickName)
{
    m_message->SetToNickName(toNickName);
}


//#pragma mark - MSLivMessage

MSLivMessage::MSLivMessage()
: m_message(new MSMessage())
{
}

MSLivMessage::~MSLivMessage()
{
    if (m_message)
    {
        delete m_message;
        m_message = nullptr;
    }
}

int MSLivMessage::GetPush()
{
    return m_message->GetPush();
}

void MSLivMessage::SetPush(int push)
{
    m_message->SetPush(push);
}

int MSLivMessage::GetFlag()
{
    return m_message->GetFlag();
}

void MSLivMessage::SetFlag(int flag)
{
    m_message->SetFlag(flag);
}

std::string& MSLivMessage::GetGroupId()
{
    return m_message->GetGroupId();
}

void MSLivMessage::SetGroupId(const std::string& groupId)
{
    m_message->SetGroupId(groupId);
}

std::string& MSLivMessage::GetToId()
{
    return m_message->GetToId();
}

void MSLivMessage::SetToId(const std::string& toId)
{
    m_message->SetToId(toId);
}

std::string& MSLivMessage::GetToNickName()
{
    return m_message->GetToNickName();
}

void MSLivMessage::SetToNickName(const std::string& toNickName)
{
    m_message->SetToNickName(toNickName);
}

//#pragma mark - MSRenMessage

MSRenMessage::MSRenMessage()
: m_message(new MSMessage())
{
}

MSRenMessage::~MSRenMessage()
{
    if (m_message)
    {
        delete m_message;
        m_message = nullptr;
    }
}

int MSRenMessage::GetPush()
{
    return m_message->GetPush();
}

void MSRenMessage::SetPush(int push)
{
    m_message->SetPush(push);
}

std::string& MSRenMessage::GetGroupId()
{
    return m_message->GetGroupId();
}

void MSRenMessage::SetGroupId(const std::string& groupId)
{
    m_message->SetGroupId(groupId);
}

std::string& MSRenMessage::GetToId()
{
    return m_message->GetToId();
}

void MSRenMessage::SetToId(const std::string& toId)
{
    m_message->SetToId(toId);
}

std::string& MSRenMessage::GetCash()
{
    return m_message->GetCash();
}

void MSRenMessage::SetCash(const std::string& cash)
{
    m_message->SetCash(cash);
}

std::string& MSRenMessage::GetWishcont()
{
    return m_message->GetWishcont();
}

void MSRenMessage::SetWishcont(const std::string& wishcont)
{
    m_message->SetWishcont(wishcont);
}

std::string& MSRenMessage::GetToNickName()
{
    return m_message->GetToNickName();
}

void MSRenMessage::SetToNickName(const std::string& toNickName)
{
    m_message->SetToNickName(toNickName);
}

//#pragma mark - MSBlkMessage

MSBlkMessage::MSBlkMessage()
: m_message(new MSMessage())
{
}

MSBlkMessage::~MSBlkMessage()
{
    if (m_message)
    {
        delete m_message;
        m_message = nullptr;
    }
}

int MSBlkMessage::GetPush()
{
    return m_message->GetPush();
}

void MSBlkMessage::SetPush(int push)
{
    m_message->SetPush(push);
}

int MSBlkMessage::GetFlag()
{
    return m_message->GetFlag();
}

void MSBlkMessage::SetFlag(int flag)
{
    m_message->SetFlag(flag);
}

std::string& MSBlkMessage::GetGroupId()
{
    return m_message->GetGroupId();
}

void MSBlkMessage::SetGroupId(const std::string& groupId)
{
    m_message->SetGroupId(groupId);
}

std::string& MSBlkMessage::GetToId()
{
    return m_message->GetToId();
}

void MSBlkMessage::SetToId(const std::string& toId)
{
    m_message->SetToId(toId);
}

std::string& MSBlkMessage::GetToNickName()
{
    return m_message->GetToNickName();
}

void MSBlkMessage::SetToNickName(const std::string& toNickName)
{
    m_message->SetToNickName(toNickName);
}

//#pragma mark - MSFbdMessage

MSFbdMessage::MSFbdMessage()
: m_message(new MSMessage())
{
}

MSFbdMessage::~MSFbdMessage()
{
    if (m_message)
    {
        delete m_message;
        m_message = nullptr;
    }
}

int MSFbdMessage::GetPush()
{
    return m_message->GetPush();
}

void MSFbdMessage::SetPush(int push)
{
    m_message->SetPush(push);
}

int MSFbdMessage::GetFlag()
{
    return m_message->GetFlag();
}

void MSFbdMessage::SetFlag(int flag)
{
    m_message->SetFlag(flag);
}

std::string& MSFbdMessage::GetGroupId()
{
    return m_message->GetGroupId();
}

void MSFbdMessage::SetGroupId(const std::string& groupId)
{
    m_message->SetGroupId(groupId);
}

std::string& MSFbdMessage::GetToId()
{
    return m_message->GetToId();
}

void MSFbdMessage::SetToId(const std::string& toId)
{
    m_message->SetToId(toId);
}

std::string& MSFbdMessage::GetToNickName()
{
    return m_message->GetToNickName();
}

void MSFbdMessage::SetToNickName(const std::string& toNickName)
{
    m_message->SetToNickName(toNickName);
}

//#pragma mark - MSMgrMessage

MSMgrMessage::MSMgrMessage()
: m_message(new MSMessage())
{
}

MSMgrMessage::~MSMgrMessage()
{
    if (m_message)
    {
        delete m_message;
        m_message = nullptr;
    }
}

int MSMgrMessage::GetPush()
{
    return m_message->GetPush();
}

void MSMgrMessage::SetPush(int push)
{
    m_message->SetPush(push);
}

int MSMgrMessage::GetFlag()
{
    return m_message->GetFlag();
}

void MSMgrMessage::SetFlag(int flag)
{
    m_message->SetFlag(flag);
}

std::string& MSMgrMessage::GetGroupId()
{
    return m_message->GetGroupId();
}

void MSMgrMessage::SetGroupId(const std::string& groupId)
{
    m_message->SetGroupId(groupId);
}

std::string& MSMgrMessage::GetToId()
{
    return m_message->GetToId();
}

void MSMgrMessage::SetToId(const std::string& toId)
{
    m_message->SetToId(toId);
}

std::string& MSMgrMessage::GetToNickName()
{
    return m_message->GetToNickName();
}

void MSMgrMessage::SetToNickName(const std::string& toNickName)
{
    m_message->SetToNickName(toNickName);
}
