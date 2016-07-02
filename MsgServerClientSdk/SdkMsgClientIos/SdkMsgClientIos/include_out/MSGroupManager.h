//
//  MSGroupManager.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSGroupManager_h
#define MSGroupManager_h
#import <Foundation/Foundation.h>
#import "MSGroupDelegate.h"

@interface MSGroupManager : NSObject

-(void)addDelegateId:(id<MSGroupDelegate>)grpDelegate
       delegateQueue:(dispatch_queue_t)grpQueue;

-(void)delDelegateId:(id<MSGroupDelegate>)grpDelegate;

-(void)addGroupGrpId:(NSString*)grpId;

-(void)rmvGroupGrpId:(NSString*)grpId;

@end

#endif /* MSGroupManager_h */
