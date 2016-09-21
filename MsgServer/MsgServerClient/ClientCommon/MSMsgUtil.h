//
//  MSMsgUtil.h
//  SdkMsgClientIos
//
//  Created by hp on 7/12/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSMsgUtil_h
#define MSMsgUtil_h

#include "MSSubMessage.h"

class MSMsgUtil{

public:
    static MSMessage*  EncodeMessageWithTxtMsg(MSTxtMessage* txtMsg, int msgType);
    static MSMessage*  EncodeMessageWithLivMsg(MSLivMessage* livMsg, int msgType);
    static MSMessage*  EncodeMessageWithRenMsg(MSRenMessage* renMsg, int msgType);
    static MSMessage*  EncodeMessageWithBlkMsg(MSBlkMessage* blkMsg, int msgType);
    static MSMessage*  EncodeMessageWithFbdMsg(MSFbdMessage* fbdMsg, int msgType);
    static MSMessage*  EncodeMessageWithMgrMsg(MSMgrMessage* mgrMsg, int msgType);

    static MSMessage*  JsonToMessage(const std::string& jsonMsg);
    static std::string*    MessageToJson(MSMessage& msg);
};

#endif /* MSMsgUtil_h */
