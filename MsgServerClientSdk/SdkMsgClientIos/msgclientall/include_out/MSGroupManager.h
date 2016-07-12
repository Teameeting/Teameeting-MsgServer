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

/**
 *  add group delegate
 *
 *  params:
 */
-(void)addDelegateId:(id<MSGroupDelegate>)grpDelegate
       delegateQueue:(dispatch_queue_t)grpQueue;

/**
 *  remove group delegate
 *
 *  params:
 */
-(void)delDelegateId:(id<MSGroupDelegate>)grpDelegate;

/**
 *  add to group
 *
 *  params:
 */
-(void)addGroupGrpId:(NSString*)grpId;

/**
 *  remove from group
 *
 *  params:
 */
-(void)rmvGroupGrpId:(NSString*)grpId;

@end

#endif /* MSGroupManager_h */
