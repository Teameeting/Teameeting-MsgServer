//
//  MSMsgSender.m
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSMsgSender.h"
#import "MsgClient.h"

@implementation MSMsgSender {
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
    //msgClient = new MsgClient(protocol);
    //if (!msgClient) {
    //    return -1;
    //}
    //return msgClient->MCInit(msgClient, [uid UTF8String], [token UTF8String], [nname UTF8String], [server UTF8String], port);
    return MsgClient::Instance().MCInit([uid UTF8String], [token UTF8String], [nname UTF8String]);
}

- (int) tMUint
{
    MsgClient::Instance().MCUnin();
    //msgClient->MCUnin();
    //if (msgClient != NULL) {
    //    delete msgClient;
    //    msgClient = NULL;
    //}
    return 0;
}

- (MCConnState) tMConnStatus
{
    return MCConnStateNOT_CONNECTED;
    //return //MsgClient::Instance().MCConnStatus();
    //if (msgClient) {
    //    return (MCConnState)msgClient->MCConnStatus();
    //} else {
    //    return MCConnStateNOT_CONNECTED;
    //}
}

- (int) tMSndMsgRoomid:(NSString*) roomid
                 rname:(NSString*) rname
                   msg:(NSString*) msg
{
    if (msgClient) {
        return -1;
        //return msgClient->MCSndMsg([roomid UTF8String], [rname UTF8String], [msg UTF8String]);
    } else {
        return -1;
    }
}

- (int) tMGetMsgCmd:(EMsgTag) tag
{
    if (msgClient) {
        return -1;//msgClient->MCGetMsg(tag);
    } else {
        return -1;
    }
}

- (int) tMOptRoomCmd:(EMsgTag) tag
              roomid:(NSString*) roomid
               rname:(NSString*) rname
              remain:(NSString*) remain
{
    if (msgClient) {
        return -1;//msgClient->MCOptRoom(tag, [roomid UTF8String], [rname UTF8String], [remain UTF8String]);
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
                     tags:(EMsgTag) tag
                      msg:(NSString*) msg
{
    if (msgClient) {
        return -1;//msgClient->MCNotifyMsg([roomid UTF8String], [rname UTF8String], tag, [msg UTF8String]);
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
