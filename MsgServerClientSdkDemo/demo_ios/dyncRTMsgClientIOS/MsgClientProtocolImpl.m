//
//  MsgClientProtocolImpl.m
//  MsgClientIos
//
//  Created by hp on 12/27/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MsgClientProtocolImpl.h"

@implementation MsgClientProtocolImpl

@synthesize delegate = _delegate;

/**
 *  callback here
 */

- (void) OnLoginCode:(int) code
{
    NSLog(@"TMMsgSender::OnLogin Code:%i\n", code);
    [self.delegate resultDisplayCallback:@"" mMsg:@"" nInt:code];
}

- (void) OnSndMsgMsg:(NSString*) msg
{
    NSLog(@"TMMsgSender::OnSndMsg msg:%@\n", msg);
    [self.delegate resultDisplayCallback:@"" mMsg:msg nInt:0];
}

- (void) OnGetMsgFrom:(NSString*) from msg:(NSString*) msg
{
    NSLog(@"TMMsgSender::OnGetMsg from:%@, msg:%@\n", from, msg);
    [self.delegate resultDisplayCallback:from mMsg:msg nInt:0];
}

- (void) OnLogoutCode:(int) code
{
    NSLog(@"TMMsgSender::OnLogout Code:%i\n", code);
    [self.delegate resultDisplayCallback:@"" mMsg:@"" nInt:code];
}

@end