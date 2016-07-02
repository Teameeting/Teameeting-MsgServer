//
//  MSSharedSeqnConfig.mm
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSSharedSeqnConfig.h"

@implementation MSSharedSeqnConfig

+(void)addGroupSeqnGroupId:(NSString*)groupId
                      Seqn:(NSNumber*)seqn
{
    NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
    [userDef setObject:seqn forKey:groupId];
    [userDef synchronize];
    NSLog(@"addGroupSeqnGroupId %@, %lld, get seqn: %lld", groupId, seqn.longLongValue, [[userDef objectForKey:groupId] longLongValue]);
}

+(void)delGroupSeqnGroupId:(NSString*)groupId
{
    NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
    [userDef removeObjectForKey:groupId];
    id obj = [userDef objectForKey:groupId];
    if (!obj) {
        NSLog(@"delGroupSeqnGroupId %@ ok", groupId);
    } else {
        NSLog(@"delGroupSeqnGroupId %@ failed", groupId);
    }
}

+(void)updateGroupSeqnGroupId:(NSString*)groupId
                         Seqn:(NSNumber*)seqn
{
    NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
    [userDef setObject:seqn forKey:groupId];
    [userDef synchronize];
    NSLog(@"updateGroupSeqnGroupId %@, %lld, get seqn: %lld", groupId, seqn.longLongValue, [[userDef objectForKey:groupId] longLongValue]);
}


+(void)addUserSeqnUserId:(NSString*)userId
                    Seqn:(NSNumber*)seqn
{
    NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
    [userDef setObject:seqn forKey:userId];
    [userDef synchronize];
    NSLog(@"addUserSeqnUserId %@, %lld, get seqn: %lld", userId, seqn.longLongValue, [[userDef objectForKey:userId] longLongValue]);
}

+(void)delUserSeqnUserId:(NSString*)userId
{
    NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
    [userDef removeObjectForKey:userId];
    id obj = [userDef objectForKey:userId];
    if (!obj) {
        NSLog(@"delUserSeqnUserId %@ ok", userId);
    } else {
        NSLog(@"delUserSeqnUserId %@ failed", userId);
    }
}

+(void)updateUserSeqnUserId:(NSString*)userId
                       Seqn:(NSNumber*)seqn
{
    NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
    [userDef setObject:seqn forKey:userId];
    [userDef synchronize];
    NSLog(@"updateUserSeqnUserId %@, %lld, get seqn: %lld", userId, seqn.longLongValue, [[userDef objectForKey:userId] longLongValue]);
}

+(void)addGroupIdGrpId:(NSString*)grpId
{
    NSUserDefaults *userDef = [NSUserDefaults standardUserDefaults];
    id i = [userDef objectForKey:@"groupNames"];
    if (i == nullptr)
    {
        
    }
    NSMutableArray *mutArray = [[NSMutableArray alloc] initWithObjects:i, nil];
    NSMutableArray *mutArray2 = [[NSMutableArray alloc] initWithArray:i];
    [mutArray addObject:grpId];
    [userDef setObject:mutArray forKey:@"groupNames"];
}

+(void)delGroupIdGrpId:(NSString*)grpId
{
    
}

@end