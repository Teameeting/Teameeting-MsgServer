//
//  MSGroupManager.m
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclient/MSGroupManager.h"
#import "msgclient/MsgClient.h"

@implementation MSGroupManager

-(void)addDelegateId:(id<MSGroupDelegate>)grpDelegate
       delegateQueue:(dispatch_queue_t)grpQueue
{
    MsgClient::Instance().MCSetGroupDelegate(grpDelegate);
}

-(void)delDelegateId:(id<MSGroupDelegate>)grpDelegate
{
    MsgClient::Instance().MCSetGroupDelegate(nullptr);
}

-(void)addGroupGrpId:(NSString*)grpId
{
    if (nil==grpId) return;
    MsgClient::Instance().MCAddGroup([grpId UTF8String]);
}

-(void)rmvGroupGrpId:(NSString*)grpId
{
    if (nil==grpId) return;
    MsgClient::Instance().MCRmvGroup([grpId UTF8String]);
}

@end
