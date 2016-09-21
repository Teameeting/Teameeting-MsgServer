//
//  MSSqlite3.h
//  SdkMsgClientIos
//
//  Created by hp on 6/28/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSSqlite3_h
#define MSSqlite3_h

#import <Foundation/Foundation.h>
#import "sqlite3.h"

@interface MSSqlite3DB : NSObject;

@property (nonatomic, assign) sqlite3* sql3Db;

- (BOOL)openDb;

- (BOOL)closeDb;

- (BOOL)isUserExistsInDbUserId:(NSString*)userId
                        SeqnId:(NSString*)seqnId;

- (BOOL)insertSeqnUserId:(NSString*)userId
                  SeqnId:(NSString*)seqnId
                 seqn:(NSNumber*)seqn;

- (BOOL)updateSeqnUserId:(NSString*)userId
                  SeqnId:(NSString*)seqnId
                    seqn:(NSNumber*)seqn;

- (BOOL)updateSeqnUserId:(NSString*)userId
                  SeqnId:(NSString*)seqnId
                    seqn:(NSNumber*)seqn
               isfetched:(NSNumber*)isfetched;

- (BOOL)selectSeqnUserId:(NSString*)userId
                  SeqnId:(NSString*)seqnId
                    seqn:(NSNumber**)seqn;

- (BOOL)deleteSeqnUserId:(NSString*)userId
                  SeqnId:(NSString*)seqnId;


- (BOOL)insertGroupIdUserId:(NSString*)userId
                      GrpId:(NSString*)grpId;

- (BOOL)isGroupExistsUserId:(NSString*)userId
                      GrpId:(NSString*)grpId;

- (BOOL)selectGroupInfoUserId:(NSString*)userId
                      GrpInfo:(NSMutableArray**)Info;

- (BOOL)deleteGroupIdUserId:(NSString*)userId
                      GrpId:(NSString*)grpId;

@end

#endif /* MSSqlite3_h */
