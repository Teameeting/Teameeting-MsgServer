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

-(BOOL)isUserExistsUserId:(NSString*)userId
                   seqnId:(NSString*)seqnId;

-(BOOL)isGroupExistsUserId:(NSString*)userId
                     GrpId:(NSString*)grpId;

-(void)addUserId:(NSString*)userId;

-(void)updateUserSeqnUserId:(NSString*)userId seqn:(NSNumber*)seqn;

-(NSNumber*)getUserSeqnUserId:(NSString*)userId;



-(void)addGroupIdUserId:(NSString*)userId
                  GrpId:(NSString*)groupId;

-(void)addGroupSeqnUserId:(NSString*)userId
                    GrpId:(NSString*)grpId
                     seqn:(NSNumber*)seqn;

-(void)updateGroupSeqnUserId:(NSString*)userId
                       GrpId:(NSString*)grpId
                        seqn:(NSNumber*)seqn;

-(void)updateGroupInfoUserId:(NSString*)userId
                       GrpId:(NSString*)grpId
                       seqn:(NSNumber*)seqn
                  isfetched:(NSNumber*)isfetched;

-(NSNumber*)getGroupSeqnUserId:(NSString*)userId
                         GrpId:(NSString*)grpId;

-(NSArray*)getGroupInfoUserId:(NSString*)userId;

-(void)delGroupIdUserId:(NSString*)userId
                  GrpId:(NSString*)groupId;

@end

#endif /* MSSqlite3Manager_h */
