//
//  MSSqlite3Manager.h
//  SdkMsgClientIos
//
//  Created by hp on 7/1/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSSqlite3Manager_h
#define MSSqlite3Manager_h
#import <Foundation/Foundation.h>
#import "msgclient/MSSqlite3.h"

@interface MSSqlite3Manager : NSObject;

@property (nonatomic, retain) MSSqlite3DB *sqlite3Db;

-(void)initManager;

-(void)uninManager;

-(BOOL)isUserExists:(NSString*)userId;

-(BOOL)isGroupExists:(NSString*)grpId;

-(void)addUserId:(NSString*)userId;

-(void)updateUserSeqnUserId:(NSString*)userId seqn:(NSNumber*)seqn;

-(NSNumber*)getUserSeqnUserId:(NSString*)userId;



-(void)addGroupId:(NSString*)groupId;

-(void)addGroupSeqnGrpId:(NSString*)grpId
                       seqn:(NSNumber*)seqn;

-(void)updateGroupSeqnGrpId:(NSString*)grpId
                       seqn:(NSNumber*)seqn;

-(void)updateGroupInfoGrpId:(NSString*)grpId
                       seqn:(NSNumber*)seqn
                  isfetched:(NSNumber*)isfetched;

-(NSNumber*)getGroupSeqnGrpId:(NSString*)grpId;

-(NSArray*)getGroupInfo;

-(void)delGroupId:(NSString*)groupId;

@end

#endif /* MSSqlite3Manager_h */
