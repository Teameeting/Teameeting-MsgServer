//
//  MSSqlite3Manager.mm
//  SdkMsgClientIos
//
//  Created by hp on 7/1/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "MSSqlite3Manager.h"

@implementation MSSqlite3Manager

@synthesize sqlite3Db = _sqlite3Db;

-(void)initManager
{
    _sqlite3Db = [[MSSqlite3DB alloc] init];
    [_sqlite3Db openDb];
}

-(void)uninManager
{
    [_sqlite3Db closeDb];
}

-(void)addUserId:(NSString*)userId
{
    [_sqlite3Db insertSeqnSeqnId:userId seqn:[NSNumber numberWithLongLong:0]];
}

-(void)updateUserSeqnUserId:(NSString*)userId seqn:(NSNumber*)seqn
{
    [_sqlite3Db updateSeqnSeqnId:userId seqn:seqn];
}

-(NSNumber*)getUserSeqnUserId:(NSString*)userId
{
    NSNumber *seqn = nil;
    [_sqlite3Db selectSeqnSeqnId:userId seqn:&seqn];
    NSLog(@"getUserSeqnUserId seqn:%lld, for user:%@", [seqn longLongValue], userId);
    return seqn;
}

-(void)addGroupId:(NSString*)groupId
{
    [_sqlite3Db insertGroupIdGrpId:groupId];
}

-(void)addGroupSeqnGrpId:(NSString*)grpId
                       seqn:(NSNumber*)seqn
{
    [_sqlite3Db insertSeqnSeqnId:grpId seqn:seqn];
}

-(void)updateGroupSeqnGrpId:(NSString*)grpId
                       seqn:(NSNumber*)seqn
{
    [_sqlite3Db updateSeqnSeqnId:grpId seqn:seqn];
}

-(NSNumber*)getGroupSeqnGrpId:(NSString*)grpId
{
    NSNumber *seqn = nil;
    [_sqlite3Db selectSeqnSeqnId:grpId seqn:&seqn];
    NSLog(@"getGroupSeqnGrpId seqn:%lld, for groupid:%@", [seqn longLongValue], grpId);
    return seqn;
}

-(NSArray*)getGroupIds
{
    NSArray *arr = nil;
    [_sqlite3Db selectGroupIds:&arr];
    for (NSString* s in arr)
    {
        NSLog(@"getGroupId id:%@", s);
    }
    return arr;
}

-(void)delGroupId:(NSString*)groupId
{
    [_sqlite3Db deleteSeqnSeqnId:groupId];
    [_sqlite3Db deleteGroupIdGrpId:groupId];
}

@end