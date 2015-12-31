//
//  TMMsgSender.m
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "TMMsgSender.h"
#import "MsgClient.h"

@implementation TMMsgSender {
    MsgClient *msgClient;
}

- (id) init {
    if (self = [super init]) {
        
    }
    return self;
}

- (void) dealloc {
    [super dealloc];
}

- (MsgClient*) getMsgClient
{
    return msgClient;
}

- (int) tMInitMsgProtocol:(id<MsgClientProtocol>) protocol
                   server:(NSString*) server
                    port :(int) port
{
    msgClient = new MsgClient(protocol);
    if (!msgClient) {
        return -1;
    }
    return msgClient->MCInit(*msgClient, [server UTF8String], port);
}

- (int) tMUint
{
    msgClient->MCUnin();
    if (msgClient != NULL) {
        delete msgClient;
        msgClient = NULL;
    }
    return 0;
}

- (int) tMConnStatus
{
    if (msgClient) {
        return msgClient->MCConnStatus();
    } else {
        return -1;
    }
}

- (int) tMLoginUserid:(NSString*) userid
                 pass:(NSString*) pass
{
    if (msgClient) {
        return msgClient->MCLogin([userid UTF8String], [pass UTF8String]);
    } else {
        return -1;
    }
    
}

- (int) tMSndMsgUserid:(NSString*) userid
                  pass:(NSString*) pass
                roomid:(NSString*) roomid
                   msg:(NSString*) msg
{
    if (msgClient) {
        return msgClient->MCSndMsg([userid UTF8String], [pass UTF8String], [roomid UTF8String], [msg UTF8String]);
    } else {
        return -1;
    }
    
}

- (int) tMGetMsgUserid:(NSString*) userid
                  pass:(NSString*) pass
{
    if (msgClient) {
        return msgClient->MCGetMsg([userid UTF8String], [pass UTF8String]);
    } else {
        return -1;
    }
    
}

- (int) tMLogoutUserid:(NSString*) userid
                  pass:(NSString*) pass
{
    if (msgClient) {
        return msgClient->MCLogout([userid UTF8String], [pass UTF8String]);
    } else {
        return -1;
    }
    
}

- (int) tMCreateRoomUserid:(NSString*) userid
                      pass:(NSString*) pass
                    roomid:(NSString*) roomid
{
    if (msgClient) {
        return msgClient->MCCreateRoom([userid UTF8String], [pass UTF8String], [roomid UTF8String]);
    } else {
        return -1;
    }
    
}

- (int) tMEnterRoomUserid:(NSString*) userid
                     pass:(NSString*) pass
                   roomid:(NSString*) roomid
{
    if (msgClient) {
        return msgClient->MCEnterRoom([userid UTF8String], [pass UTF8String], [roomid UTF8String]);
    } else {
        return -1;
    }
    
}

- (int) tMLeaveRoomUserid:(NSString*) userid
                     pass:(NSString*) pass
                   roomid:(NSString*) roomid
{
    if (msgClient) {
        return msgClient->MCLeaveRoom([userid UTF8String], [pass UTF8String], [roomid UTF8String]);
    } else {
        return -1;
    }
    
}

- (int) tMDestroyRoomUserid:(NSString*) userid
                       pass:(NSString*) pass
                     roomid:(NSString*) roomid
{
    if (msgClient) {
        return msgClient->MCDestroyRoom([userid UTF8String], [pass UTF8String], [roomid UTF8String]);
    } else {
        return -1;
    }
}

- (int) tMSndMsgToUserid:(NSString*) userid
                    pass:(NSString*) pass
                  roomid:(NSString*) roomid
                     msg:(NSString*) msg
                   ulist:(NSArray*) ulist
{
    //return msgClient->MCCreateRoom([userid UTF8String], [pass UTF8String], [roomid UTF8String]);
    return 0;
}
@end