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

-(BOOL)isUserExistsUserId:(NSString*)userId
                   seqnId:(NSString*)seqnId
{
    return [_sqlite3Db isUserExistsInDbUserId:userId SeqnId:seqnId];
}

-(BOOL)isGroupExistsUserId:(NSString*)userId
                     GrpId:(NSString*)grpId
{
    return [_sqlite3Db isGroupExistsUserId:userId GrpId:grpId];
}

-(void)addUserId:(NSString*)userId
{
    [_sqlite3Db insertSeqnUserId:userId SeqnId:userId seqn:[NSNumber numberWithLongLong:0]];
}

-(void)updateUserSeqnUserId:(NSString*)userId seqn:(NSNumber*)seqn
{
    [_sqlite3Db updateSeqnUserId:userId SeqnId:userId seqn:seqn];
}

-(NSNumber*)getUserSeqnUserId:(NSString*)userId
{
    NSNumber *seqn = nil;
    [_sqlite3Db selectSeqnUserId:userId SeqnId:userId seqn:&seqn];
    return seqn;
}

-(void)addGroupIdUserId:(NSString*)userId
                  GrpId:(NSString*)groupId
{
    [_sqlite3Db insertGroupIdUserId:userId GrpId:groupId];
}

-(void)addGroupSeqnUserId:(NSString*)userId
                    GrpId:(NSString*)grpId
                     seqn:(NSNumber*)seqn
{
    [_sqlite3Db insertSeqnUserId:userId SeqnId:grpId seqn:seqn];
}

-(void)updateGroupSeqnUserId:(NSString*)userId
                       GrpId:(NSString*)grpId
                        seqn:(NSNumber*)seqn
{
    [_sqlite3Db updateSeqnUserId:userId SeqnId:grpId seqn:seqn];
}

-(void)updateGroupInfoUserId:(NSString*)userId
                       GrpId:(NSString*)grpId
                        seqn:(NSNumber*)seqn
                   isfetched:(NSNumber*)isfetched
{
    [_sqlite3Db updateSeqnUserId:userId SeqnId:grpId seqn:seqn isfetched:isfetched];
}

-(NSNumber*)getGroupSeqnUserId:(NSString*)userId
                         GrpId:(NSString*)grpId
{
    NSNumber *seqn = nil;
    [_sqlite3Db selectSeqnUserId:userId SeqnId:grpId seqn:&seqn];
    return seqn;
}

-(NSArray*)getGroupInfoUserId:(NSString*)userId
{
    NSArray *arr = nil;
    [_sqlite3Db selectGroupInfoUserId:userId GrpInfo:&arr];
    return arr;
}

-(void)delGroupIdUserId:(NSString*)userId
                  GrpId:(NSString*)groupId
{
    [_sqlite3Db deleteSeqnUserId:userId SeqnId:groupId];
    [_sqlite3Db deleteGroupIdUserId:userId GrpId:groupId];
}

@end
