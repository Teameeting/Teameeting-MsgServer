//
//  MSMessage.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSMessage_h
#define MSMessage_h

#include <string>
#include "MSClientType.h"

class MSMessage {

public:

    MSMessage();
    void            SetContent(const std::string& c) { content = c; }
    std::string&    GetContent() { return content; }
    void            SetExtra(const std::string& e) { extra = e; }
    std::string&    GetExtra() { return extra; }
    void            SetMsgId(const std::string& m) { msgId = m; }
    std::string&    GetMsgId() { return msgId; }
    void            SetToId(const std::string& t) { toId = t; }
    std::string&    GetToId() { return toId; }
    void            SetFromId(const std::string& f) { fromId = f; }
    std::string&    GetFromId() { return fromId; }
    void            SetGroupId(const std::string& g) { groupId = g; }
    std::string&    GetGroupId() { return groupId; }
    void            SetNickName(const std::string& n) { nickName = n; }
    std::string&    GetNickName() { return nickName; }
    void            SetUiconUrl(const std::string& u) { uiconUrl = u; }
    std::string&    GetUiconUrl() { return uiconUrl; }
    void            SetCash(const std::string& c) { cash = c; }
    std::string&    GetCash() { return cash; }
    void            SetWishcont(const std::string& w) { wishcont = w; }
    std::string&    GetWishcont() { return wishcont; }
    void            SetToNickName(const std::string& t) { toNickName = t; }
    std::string&    GetToNickName() { return toNickName; }
    void            SetMillSec(unsigned int m) { millSec = m; }
    unsigned int    GetMillSec() { return millSec; }
    void            SetMsgType(MCMsgType m) { msgType = m; }
    MCMsgType       GetMsgType() { return msgType; }
    void            SetFlag(int f) { flag = f; }
    int             GetFlag() { return flag; }
    void            SetPush(int p) { push = p; }
    int             GetPush() { return push; }

private:
    std::string     content;
    std::string     extra;
    std::string     msgId;
    std::string     toId;
    std::string     fromId;
    std::string     groupId;
    std::string     nickName;
    std::string     uiconUrl;
    std::string     cash;
    std::string     wishcont;
    std::string     toNickName;
    unsigned int    millSec;
    MCMsgType       msgType;
    int             flag;
    int             push;

};

#endif /* MSMessage_h */
