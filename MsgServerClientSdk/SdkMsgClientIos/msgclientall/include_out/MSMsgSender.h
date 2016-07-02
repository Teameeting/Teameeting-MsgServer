//
//  MSMsgSender.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#ifndef MsgClientIos_MSMsgSender_h
#define MsgClientIos_MSMsgSender_h
#import "MsgClientProtocol.h"
#import "MSClientType.h"
#import "MeeMSsgType.pbobjc.h"

/**
 * MSMsgSender is used to send msg to other
 **/

@interface MSMsgSender : NSObject

/**
 * init
 *
 * params:
 *      protocol: the callback for msgclient
 *      uid:user identifier, it can be uuid or device id
 *      token:token from http server
 *      server:server ip
 *      port:server port
 **/
- (int) MSIniMSsgProtocol:(id<MsgClientProtocol>)protocol
                      uid:(NSString*) uid
                    token:(NSString*) token
                    nname:(NSString*) nname
                   server:(NSString*) server
                    port :(int) port;
/**
 * unin
 **/
- (int) MSUint;

/**
 * the status of connection between msgclient and msgserver
 **/
- (MCConnState) MSConnStatus;

/**
 * send msg to all member in the meeting room
 * params:
 *      roomid:the room id
 *      msg:the msg to send
 **/
- (int) MSSndMsgRoomid:(NSString*) roomid
                 rname:(NSString*) rname
                   msg:(NSString*) msg;

/**
 * get msg from server
 * params:
        cmd:
 **/
- (int) MSGeMSsgCmd:(EMsgTag) tag;

/**
 * operation for room
 * params:
 *      cmd:the cmd for room, enter or leave
 *      roomid:the room id
 *      remain:remain
 **/
- (int) MSOptRoomCmd:(EMsgTag) tag
              roomid:(NSString*) roomid
               rname:(NSString*) rname
              remain:(NSString*) remain;

/**
 * send msg to some member in meeting room(not used now)
 * params:
 *      roomid:the room id
 *      msg:the msg to send
 *      ulist:the members to send to
 **/
- (int) MSSndMsgToRoomid:(NSString*) roomid
                   rname:(NSString*) rname
                     msg:(NSString*) msg
                   ulist:(NSArray*) ulist;

/**
 * notify others with self publish id
 * params:
 *      roomid:the room id
 *      msg:the publish id
 **/
- (int) MSNotifyMsgRoomid:(NSString*) roomid
                    rname:(NSString*) rname
                     tags:(EMsgTag) tag
                      msg:(NSString*) msg;

/**
 * set user nick name
 * params:
 *      nname: nick name
 **/
- (void) MSSetNickNameNname:(NSString*) nname;

@end

#endif
