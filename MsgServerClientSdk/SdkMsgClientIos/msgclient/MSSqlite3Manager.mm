//
//  MSSqlite3Manager.mm
//  SdkMsgClientIos
//
//  Created by hp on 7/1/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclient/MSSqlite3Manager.h"

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

-(BOOL)isUserExists:(NSString*)userId
{
    return [_sqlite3Db isUserExistsInDb:userId];
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

-(void)updateGroupInfoGrpId:(NSString*)grpId
                       seqn:(NSNumber*)seqn
                  isfetched:(NSNumber*)isfetched
{
    [_sqlite3Db updateSeqnSeqnId:grpId seqn:seqn isfetched:isfetched];
}

-(NSNumber*)getGroupSeqnGrpId:(NSString*)grpId
{
    NSNumber *seqn = nil;
    [_sqlite3Db selectSeqnSeqnId:grpId seqn:&seqn];
    return seqn;
}

-(NSArray*)getGroupInfo
{
    NSArray *arr = nil;
    [_sqlite3Db selectGroupInfo:&arr];
    return arr;
}

-(void)delGroupId:(NSString*)groupId
{
    [_sqlite3Db deleteSeqnSeqnId:groupId];
    [_sqlite3Db deleteGroupIdGrpId:groupId];
}

@end
