//
//  MSMsgSender.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_MSClientType_h
#define MsgClientIos_MSClientType_h

typedef NS_ENUM(NSInteger, MCConnState){
    MCConnStateNOT_CONNECTED = 0,
    MCConnStateCONNECTTING = 1,
    MCConnStateCONNECTED = 2
};

typedef NS_ENUM(NSInteger, MCMsgType){
    MCMsgTypeTtxt = 0,
    MCMsgTypeTfil = 1,
    MCMsgTypeTpic = 2,
    MCMsgTypeTaud = 3,
    MCMsgTypeTvid = 4,
    MCMsgTypeTemj = 5,
    MCMsgTypeTsdf = 6,
    MCMsgTypeTliv = 20,
    MCMsgTypeTren = 21,
    MCMsgTypeTblk = 22,
    MCMsgTypeTfbd = 23,
    MCMsgTypeTmgr = 24
};

#endif
