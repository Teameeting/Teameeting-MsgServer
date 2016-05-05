//
//  MsgClientProtocolImpl.m
//  MsgClientIos
//
//  Created by hp on 12/27/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MsgClientProtocolImpl.h"
#import "TMClientType.h"
#import "MeetMsgType.pbobjc.h"
#import "MeetMsg.pbobjc.h"

@implementation MsgClientProtocolImpl

@synthesize delegate = _delegate;

/**
 *  callback here
 */

- (void) OnSndMsgMsg:(NSString*) msg
{    
    NSLog(@"TMMsgSender::OnSndMsg msg:%@\n", msg);
    NSData* msgData = [msg dataUsingEncoding:NSUTF8StringEncoding];
    MeetMsg* meetMsg = [MeetMsg parseFromData:msgData error:nullptr];
    NSLog(@"TMMsgSender::OnSndMsg MeetMsg:%@\n", [meetMsg debugDescription]);
    NSString* cont = [meetMsg msgCont];
    EMsgTag tag = [meetMsg msgTag];
    
#if 1
    switch (tag) {
        case EMsgTag_Tchat:
        {
            NSString* content = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: content:%@", cont]];
            [self.delegate resultDisplayCallback:@"" mMsg:content nInt:0];
        }
            break;
        case EMsgTag_Tenter:
        {
            NSString* content = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: content:%@", cont]];
            [self.delegate resultDisplayCallback:@"" mMsg:content nInt:0];
        }
            
            break;
        case EMsgTag_Tleave:
        {
            NSString* content = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: content:%@", cont]];
            [self.delegate resultDisplayCallback:@"" mMsg:content nInt:0];
        }
            
            break;
        case EMsgTag_Tnotify:
        {
            NSString* content = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: content:%@", cont]];
            [self.delegate resultDisplayCallback:@"" mMsg:content nInt:0];
        }
            
            break;
        default:
            break;
    }
#endif
}

- (void) OnGetMsgMsg:(NSString*) msg
{
    NSLog(@"TMMsgSender::OnGetMsg msg:%@\n", msg);
    [self.delegate resultDisplayCallback:@"" mMsg:msg nInt:0];
}

- (void) OnMsgServerConnected
{
    NSString *ns_msc = @"Callback OnMsgServerConnected was called";
    NSLog(@"TMMsgSender %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerDisconnect
{
    NSString *ns_msc = @"Callback OnMsgServerDisconnect was called";
    NSLog(@"TMMsgSender %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerConnectionFailure
{
    NSString *ns_msc = @"Callback OnMsgServerConnectionFailure was called";
    NSLog(@"TMMsgSender %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerStateConnState:(MCConnState) state
{
    NSString* ns_msc = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: Callback OnMsgServerStateTcpState was called state:%i", (int)state ]];
    NSLog(@"TMMsgSender %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}


@end