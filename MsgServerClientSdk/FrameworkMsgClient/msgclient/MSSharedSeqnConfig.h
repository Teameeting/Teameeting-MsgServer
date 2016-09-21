//
//  MSSharedSeqnConfig.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSSharedSeqnConfig_h
#define MSSharedSeqnConfig_h

#import <Foundation/Foundation.h>

@interface MSSharedSeqnConfig : NSObject;

+(void)addGroupSeqnGroupId:(NSString*)groupId
                      Seqn:(NSNumber*)seqn;

+(void)delGroupSeqnGroupId:(NSString*)groupId;

+(void)updateGroupSeqnGroupId:(NSString*)groupId
                         Seqn:(NSNumber*)seqn;


+(void)addUserSeqnUserId:(NSString*)userId
                    Seqn:(NSNumber*)seqn;

+(void)delUserSeqnUserId:(NSString*)userId;

+(void)updateUserSeqnUserId:(NSString*)userId
                       Seqn:(NSNumber*)seqn;

+(void)addGroupIdGrpId:(NSString*)grpId;
+(void)delGroupIdGrpId:(NSString*)grpId;

@end

#endif /* MSSharedSeqnConfig_h */
