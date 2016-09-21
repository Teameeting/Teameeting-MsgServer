//
//  MSSubMessage.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSSubMessage_h
#define MSSubMessage_h

#include "MSMessage.h"

class MSTxtMessage {
public:
    MSTxtMessage();
    ~MSTxtMessage();
    int     GetPush();
    void    SetPush(int push);
    std::string&    GetContent();
    void            SetContent(const std::string& content);
    std::string&    GetGroupId();
    void            SetGroupId(const std::string& groupId);
    std::string&    GetToId();
    void            SetToId(const std::string& toId);
    std::string&    GetToNickName();
    void            SetToNickName(const std::string& toNickName);
private:
    MSMessage   *m_message;
};

class MSLivMessage {
public:
    MSLivMessage();
    ~MSLivMessage();
    int     GetPush();
    void    SetPush(int push);
    int     GetFlag();
    void    SetFlag(int flag);
    std::string&    GetGroupId();
    void            SetGroupId(const std::string& groupId);
    std::string&    GetToId();
    void            SetToId(const std::string& toId);
    std::string&    GetToNickName();
    void            SetToNickName(const std::string& toNickName);
private:
    MSMessage   *m_message;
};

class MSRenMessage {
public:
    MSRenMessage();
    ~MSRenMessage();
    int     GetPush();
    void    SetPush(int push);
    std::string&    GetGroupId();
    void            SetGroupId(const std::string& groupId);
    std::string&    GetToId();
    void            SetToId(const std::string& toId);
    std::string&    GetCash();
    void            SetCash(const std::string& cash);
    std::string&    GetWishcont();
    void            SetWishcont(const std::string& wishcont);
    std::string&    GetToNickName();
    void            SetToNickName(const std::string& toNickName);
private:
    MSMessage   *m_message;
};

class MSBlkMessage {
public:
    MSBlkMessage();
    ~MSBlkMessage();
    int     GetPush();
    void    SetPush(int push);
    int     GetFlag();
    void    SetFlag(int flag);
    std::string&    GetGroupId();
    void            SetGroupId(const std::string& groupId);
    std::string&    GetToId();
    void            SetToId(const std::string& toId);
    std::string&    GetToNickName();
    void            SetToNickName(const std::string& toNickName);
private:
    MSMessage   *m_message;
};

class MSFbdMessage {
public:
    MSFbdMessage();
    ~MSFbdMessage();
    int     GetPush();
    void    SetPush(int push);
    int     GetFlag();
    void    SetFlag(int flag);
    std::string&    GetGroupId();
    void            SetGroupId(const std::string& groupId);
    std::string&    GetToId();
    void            SetToId(const std::string& toId);
    std::string&    GetToNickName();
    void            SetToNickName(const std::string& toNickName);
private:
    MSMessage   *m_message;
};

class MSMgrMessage {
public:
    MSMgrMessage();
    ~MSMgrMessage();
    int     GetPush();
    void    SetPush(int push);
    int     GetFlag();
    void    SetFlag(int flag);
    std::string&    GetGroupId();
    void            SetGroupId(const std::string& groupId);
    std::string&    GetToId();
    void            SetToId(const std::string& toId);
    std::string&    GetToNickName();
    void            SetToNickName(const std::string& toNickName);
private:
    MSMessage   *m_message;
};

#endif /* MSSubMessage_h */
