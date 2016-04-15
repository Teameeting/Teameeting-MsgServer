//
//  TMMsgSender.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_TMClientType_h
#define MsgClientIos_TMClientType_h

typedef NS_ENUM(NSInteger, MCConnState){
    MCConnStateNOT_CONNECTED = 0,
    MCConnStateCONNECTTING = 1,
    MCConnStateCONNECTED = 2
};

typedef NS_ENUM(NSInteger, MCMeetCmd){
    MCMeetCmdENTER=1,
    MCMeetCmdLEAVE,
    MCMeetCmdDCOMM,
    MCMeetCmdINVALID
};

typedef NS_ENUM(NSInteger, MCSendTags){
    MCSendTagsTALK=1,
    MCSendTagsENTER,
    MCSendTagsLEAVE,
    MCSendTagsCALL,
    MCSendTagsINVALID
};

typedef NS_ENUM(NSInteger, MCMessageType){
    MCMessageTypeREQUEST=1,
    MCMessageTypeRESPONSE,
    MCMessageTypeINVALID
};

typedef NS_ENUM(NSInteger, MCGetCmd){
    MCGetCmdINVALID=1
};

#endif
