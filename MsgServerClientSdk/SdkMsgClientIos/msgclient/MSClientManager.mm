//
//  MSClientManager.m
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSClientManager.h"
#import "MsgClient.h"

@implementation MSClientManager

-(int)initMsgClientUsrId:(NSString*)usrId
                   token:(NSString*)token
                   nName:(NSString*)nName
{
    NSLog(@"MsgClient Ios Version:v1.0.0");
    MsgClient::Instance().MCInit([usrId UTF8String], [token UTF8String], [nName UTF8String]);
    MsgClient::Instance().MCRegisterMsgCb(&MsgClient::Instance());
    return 0;
}

-(int)uninMsgClient
{
    MsgClient::Instance().MCUnRegisterMsgCb(&MsgClient::Instance());
    MsgClient::Instance().MCUnin();
    return 0;
}

-(void)addDelegateId:(id<MSClientDelegate>)cltDelegate
       delegateQueue:(dispatch_queue_t)cltQueue
{
    MsgClient::Instance().MCSetClientDelegate(cltDelegate);
}

-(void)delDelegateId:(id<MSClientDelegate>)cltDelegate
{
    MsgClient::Instance().MCSetClientDelegate(nullptr);
}

-(int)connToServer:(NSString*)server
              port:(int)port
{
    MsgClient::Instance().MCConnToServer([server UTF8String], port);
    return 0;
}

-(MCConnState)connStatus
{
    MsgClient::Instance().MCConnStatus();
    return MCConnStateNOT_CONNECTED;
}

@end