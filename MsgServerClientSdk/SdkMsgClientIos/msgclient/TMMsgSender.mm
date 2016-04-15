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
    
}

- (MsgClient*) getMsgClient
{
    return msgClient;
}

- (int) tMInitMsgProtocol:(id<MsgClientProtocol>) protocol
                      uid:(NSString*) uid
                    token:(NSString*) token
                    nname:(NSString*) nname
                   server:(NSString*) server
                    port :(int) port
{
    msgClient = new MsgClient(protocol);
    if (!msgClient) {
        return -1;
    }
    return msgClient->MCInit(msgClient, [uid UTF8String], [token UTF8String], [nname UTF8String], [server UTF8String], port);
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

- (MCConnState) tMConnStatus
{
    if (msgClient) {
        return (MCConnState)msgClient->MCConnStatus();
    } else {
        return MCConnStateNOT_CONNECTED;
    }
}

- (int) tMSndMsgRoomid:(NSString*) roomid
                 rname:(NSString*) rname
                   msg:(NSString*) msg
{
    if (msgClient) {
        return msgClient->MCSndMsg([roomid UTF8String], [rname UTF8String], [msg UTF8String]);
    } else {
        return -1;
    }
}

- (int) tMGetMsgCmd:(MCGetCmd) cmd
{
    if (msgClient) {
        return msgClient->MCGetMsg((GETCMD)cmd);
    } else {
        return -1;
    }
}

- (int) tMOptRoomCmd:(MCMeetCmd) cmd
              roomid:(NSString*) roomid
               rname:(NSString*) rname
              remain:(NSString*) remain
{
    if (msgClient) {
        return msgClient->MCOptRoom((MEETCMD)cmd, [roomid UTF8String], [rname UTF8String], [remain UTF8String]);
    } else {
        return -1;
    }
}

- (int) tMSndMsgToRoomid:(NSString*) roomid
                   rname:(NSString*) rname
                     msg:(NSString*) msg
                   ulist:(NSArray*) ulist
{
    return 0;
}

- (int) tMNotifyMsgRoomid:(NSString*) roomid
                    rname:(NSString*) rname
                     tags:(MCSendTags) tags
                      msg:(NSString*) msg
{
    if (msgClient) {
        return msgClient->MCNotifyMsg([roomid UTF8String], [rname UTF8String], (SENDTAGS)tags, [msg UTF8String]);
    } else {
        return -1;
    }
}

- (void) tMSetNickNameNname:(NSString*) nname
{
    if (msgClient) {
        msgClient->MCSetNickName([nname UTF8String]);
    }
}

@end