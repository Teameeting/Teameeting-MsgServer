//
//  MSMessage.mm
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#include "MSMessage.h"

MSMessage::MSMessage()
: content("")
, extra("")
, msgId("")
, toId("")
, fromId("")
, groupId("")
, nickName("")
, uiconUrl("")
, cash("")
, wishcont("")
, toNickName("")
, millSec(0)
, msgType(MCMsgTypeTtxt)
, flag(0)
, push(0){}
