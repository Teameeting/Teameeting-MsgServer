//
//  MsgClientProtocolImpl.m
//  MsgClientIos
//
//  Created by hp on 12/27/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MsgClientProtocolImpl.h"
#import "msgclient/MSClientType.h"

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
    NSLog(@"MsgClientProtocolImpl function:%@, msgid:%@, code:%d\n", [NSString stringWithUTF8String:__FUNCTION__], msgId, code);
}

// when recv text msg
-(void)OnRecvTxtMessage:(MSTxtMessage*)txtMsg
{
    NSLog(@"MsgClientProtocolImpl function:%@\n", [NSString stringWithUTF8String:__FUNCTION__]);
    NSString *message = [NSString stringWithFormat:@"fromId:%@, toId:%@, content:%@", [txtMsg fromId], [txtMsg toId], [txtMsg content]];
    [self.delegate resultDisplayCallback:message mMsg:@"" nInt:0];
}

// when recv self define msg
// not used now
-(void)OnRecvSelfDefMessage:(MSTxtMessage*)sdefMsg
{
    NSLog(@"MsgClientProtocolImpl function:%@\n", [NSString stringWithUTF8String:__FUNCTION__]);
}

// when hostId begin to live in grpId
-(void)OnNotifyLiveMessage:(MSTxtMessage*)txtMsg
{
    NSLog(@"MsgClientProtocolImpl function:%@, grpid:%@, hostid:%@\n", [NSString stringWithUTF8String:__FUNCTION__], [txtMsg groupId], [txtMsg fromId]);
}

// when hostId in grpId recv red-envelope
-(void)OnNotifyRedEnvelopeMessage:(MSTxtMessage*)txtMsg
{
    NSLog(@"MsgClientProtocolImpl function:%@, grpid:%@, hostid:%@\n", [NSString stringWithUTF8String:__FUNCTION__], [txtMsg groupId], [txtMsg fromId]);
}

// when userId was push to blacklist in grpId
-(void)OnNotifyBlacklistMessage:(MSTxtMessage*)txtMsg
{
    NSLog(@"MsgClientProtocolImpl function:%@, grpid:%@, userid:%@\n", [NSString stringWithUTF8String:__FUNCTION__], [txtMsg groupId], [txtMsg fromId]);
}

// when userId was forbidden in grpId
-(void)OnNotifyForbiddenMessage:(MSTxtMessage*)txtMsg
{
    NSLog(@"MsgClientProtocolImpl function:%@, grpid:%@, userid:%@\n", [NSString stringWithUTF8String:__FUNCTION__], [txtMsg groupId], [txtMsg fromId]);
}

// when userId in grpId was setted to be manager
-(void)OnNotifySettedMgrMessage:(MSTxtMessage*)txtMsg
{
    NSLog(@"MsgClientProtocolImpl function:%@, grpid:%@, userid:%@\n", [NSString stringWithUTF8String:__FUNCTION__], [txtMsg groupId], [txtMsg fromId]);
}

- (void) OnMsgServerConnected
{
    NSString *ns_msc = @"Callback OnMsgServerConnected was called";
    NSLog(@"MsgClientProtocolImpl %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerConnecting
{
    NSString *ns_msc = @"Callback OnMsgServerConnecting was called";
    NSLog(@"MsgClientProtocolImpl %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerDisconnect
{
    NSString *ns_msc = @"Callback OnMsgServerDisconnect was called";
    NSLog(@"MsgClientProtocolImpl %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

- (void) OnMsgServerConnectionFailure
{
    NSString *ns_msc = @"Callback OnMsgServerConnectionFailure was called";
    NSLog(@"MsgClientProtocolImpl %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

// when client initializing
-(void)OnMsgClientInitializing
{
    NSString *ns_msc = @"Callback OnMsgClientInitializing was called";
    NSLog(@"MsgClientProtocolImpl %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

// when client initialize ok
-(void)OnMsgClientInitialized
{
    NSString *ns_msc = @"Callback OnMsgClientInitialized was called";
    NSLog(@"MsgClientProtocolImpl %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
    
}

// when client un initialize
-(void)OnMsgClientUnInitialize
{
    NSString *ns_msc = @"Callback OnMsgClientUnInitialize was called";
    NSLog(@"MsgClientProtocolImpl %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

// when client initialize failed
-(void)OnMsgClientInitializeFailure
{
    NSString *ns_msc = @"Callback OnMsgClientInitializeFailure was called";
    NSLog(@"MsgClientProtocolImpl %@\n", ns_msc);
    [self.delegate resultDisplayCallback:ns_msc mMsg:@"" nInt:0];
}

@end