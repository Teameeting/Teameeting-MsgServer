//
//  MsgClientProtocol.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_MsgClientProtocol_h
#define MsgClientIos_MsgClientProtocol_h

#import "TMClientType.h"

@protocol MsgClientProtocol <NSObject>

@required

/**
 * when you invoke tMSndMsgRoomid, you will recv response on this callback
 * you can alse recv msgs from others in this callback
 * also you will recv all the 'SndMsg' callback here
 * e.g. tMOptRoomCmd(MCMeetCmdENTER, @"roomid", @"")
 * tMOptRoomCmd(MCMeetCmdLEAVE, @"roomid", @"")
 **/
- (void) OnSndMsgMsg:(NSString*) msg;

/**
 * this callback is not used now.
 **/
- (void) OnGetMsgMsg:(NSString*) msg;

/**
 * after the msgclient connect to server
 * this callback will be invoked
 **/
- (void) OnMsgServerConnected;

/**
 * when the msgclient disconnect from server
 * this callback will be invoked
 **/
- (void) OnMsgServerDisconnect;

/**
 * when the msgclient connect to server failed
 * this callback will be invoked
 **/
- (void) OnMsgServerConnectionFailure;

/**
 * when the state between server and client has changed
 * this callback will be invoked
 *
 * params:
 *      state: the state of connection between msgclient and msgserver
 **/
- (void) OnMsgServerStateConnState:(MCConnState) state;

@end

#endif
