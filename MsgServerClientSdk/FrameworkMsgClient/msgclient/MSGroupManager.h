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
#import "msgclient/MSGroupDelegate.h"

@interface MSGroupManager : NSObject

/**
 *  add group delegate
 *
 *  params:
 *      grpDelegate: the group delegate for callback to set
 *      grpQueue: the dispatch queue
 *
 *  return:
 *      void
 */
-(void)addDelegateId:(id<MSGroupDelegate>)grpDelegate
       delegateQueue:(dispatch_queue_t)grpQueue;

/**
 *  remove group delegate
 *
 *  params:
 *      grpDelegate: the group delegate to delete
 *
 *  return:
 *      void
 */
-(void)delDelegateId:(id<MSGroupDelegate>)grpDelegate;

/**
 *  add to group
 *
 *  params:
 *      grpId: the group id to add
 *
 *  return:
 *      void
 */
-(void)addGroupGrpId:(NSString*)grpId;

/**
 *  remove from group
 *
 *  params:
 *      grpId: the group id to remove
 *
 *  return:
 *      void
 */
-(void)rmvGroupGrpId:(NSString*)grpId;

@end

#endif /* MSGroupManager_h */
