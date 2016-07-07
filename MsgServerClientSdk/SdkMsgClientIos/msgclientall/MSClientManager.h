//
//  MSClientManager.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSClientManager_h
#define MSClientManager_h

#import <Foundation/Foundation.h>
#import "MSClientType.h"
#import "MSClientDelegate.h"

@interface MSClientManager : NSObject

/**
 *  init msg client
 *
 *  params:
 */
-(int)initMsgClientUsrId:(NSString*)usrId
                   token:(NSString*)token
                   nName:(NSString*)nName;

/**
 *  uninit msg client
 *
 *  params:
 */
-(int)uninMsgClient;

/**
 *  add client delegate
 *
 *  params:
 */
-(void)addDelegateId:(id<MSClientDelegate>)cltDelegate
       delegateQueue:(dispatch_queue_t)cltQueue;

/**
 *  remove client delegate
 *
 *  params:
 */
-(void)delDelegateId:(id<MSClientDelegate>)cltDelegate;

/**
 *  connect to server
 *
 *  params:
 */
-(int)connToServer:(NSString*)server
              port:(int)port;

/**
 *  get connection status
 *
 *  params:
 */
-(MCConnState)connStatus;

@end

#endif /* MSClientManager_h */
