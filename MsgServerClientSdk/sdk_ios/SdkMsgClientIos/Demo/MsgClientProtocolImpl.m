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

@implementation MsgClientProtocolImpl

@synthesize delegate = _delegate;

/**
 *  callback here
 */

- (void) OnSndMsgMsg:(NSString*) msg
{    
    NSLog(@"TMMsgSender::OnSndMsg msg:%@\n", msg);
    NSData* msgData = [msg dataUsingEncoding:NSUTF8StringEncoding];
    NSMutableDictionary* dict = [NSJSONSerialization JSONObjectWithData:msgData options:NSJSONReadingAllowFragments error:nil];
    NSNumber* messagetype = [dict objectForKey:@"messagetype"];
    NSString* cont = [dict objectForKey:@"cont"];
    NSNumber* tags = [dict objectForKey:@"tags"];

    if ([messagetype intValue]==MCMessageTypeREQUEST) {
        switch ([tags intValue]) {
            case MCSendTagsTALK:
            {
                NSString* content = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: content:%@", cont]];
                [self.delegate resultDisplayCallback:@"" mMsg:content nInt:0];
            }
                break;
            case MCSendTagsENTER:
            {
                NSString* content = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: content:%@", cont]];
                [self.delegate resultDisplayCallback:@"" mMsg:content nInt:0];
            }

                break;
            case MCSendTagsLEAVE:
            {
                NSString* content = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: content:%@", cont]];
                [self.delegate resultDisplayCallback:@"" mMsg:content nInt:0];
            }

                break;
            default:
                break;
        }
    }
    
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