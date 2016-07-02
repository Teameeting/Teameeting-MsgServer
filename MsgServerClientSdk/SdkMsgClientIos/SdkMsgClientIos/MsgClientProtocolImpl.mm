//
//  MsgClientProtocolImpl.m
//  MsgClientIos
//
//  Created by hp on 12/27/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MsgClientProtocolImpl.h"
#import "MSClientType.h"
#import "EntityMsgType.pbobjc.h"
#import "EntityMsg.pbobjc.h"

@implementation MsgClientProtocolImpl

@synthesize delegate = _delegate;

/**
 *  callback here
 */

-(void) OnAddGroupSuccessGrpId:(NSString*)grpId
{
    
}

-(void) OnAddGroupFailedGrpId:(NSString*)grpId
                       reason:(NSString*)reason
                         code:(NSInteger)code
{
    
}

// when del an group
-(void) OnRmvGroupSuccessGrpId:(NSString*)grpId
{
    
}

-(void) OnRmvGroupFailedGrpId:(NSString*)grpId
                       reason:(NSString*)reason
                         code:(NSInteger)code
{
    
}


// when the msg you send reached server
-(void)OnSendMessageId:(NSString*)msgId
                  code:(int)code
{
    
}

// when recv text msg
-(void)OnRecvTxtMessage:(MSTxtMessage*)txtMsg
{
    NSString* content = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: content:%@", [txtMsg content]]];
    [self.delegate resultDisplayCallback:@"" mMsg:content nInt:0];
}

// when recv self define msg
// not used now
-(void)OnRecvSelfDefMessage:(MSTxtMessage*)sdefMsg
{
    
}

// when hostId begin to live in grpId
-(void)OnNotifyLiveId:(NSString*)grpId
               hostId:(NSString*)hostId
{
    
}

// when hostId in grpId recv red-envelope
-(void)OnNotifyRedEnvelopeGrpId:(NSString*)grpId
                         hostId:(NSString*)hostId
{
    
}

// when userId was push to blacklist in grpId
-(void)OnNotifyBlacklist:(NSString*)grpId
                  userId:(NSString*)userId
{
    
}

// when userId was forbidden in grpId
-(void)OnNotifyForbidden:(NSString*)grpId
                  userId:(NSString*)userId
{
    
}

// when userId in grpId was setted to be manager
-(void)OnNotifySettedMgrGrpId:(NSString*)grpId
                       userId:(NSString*)userId
{
    
}

- (void) OnMsgServerConnected
{
    NSString *ns_msc = @"Callback OnMsgServerConnected was called";
    NSLog(@"MSMsgSender %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerConnecting
{
    NSString *ns_msc = @"Callback OnMsgServerConnecting was called";
    NSLog(@"MSMsgSender %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerDisconnect
{
    NSString *ns_msc = @"Callback OnMsgServerDisconnect was called";
    NSLog(@"MSMsgSender %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerConnectionFailure
{
    NSString *ns_msc = @"Callback OnMsgServerConnectionFailure was called";
    NSLog(@"MSMsgSender %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}


@end