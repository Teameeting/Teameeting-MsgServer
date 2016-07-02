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

-(int)initMsgClientUsrId:(NSString*)usrId
                   token:(NSString*)token
                   nName:(NSString*)nName;

-(int)uninMsgClient;

-(void)addDelegateId:(id<MSClientDelegate>)cltDelegate
       delegateQueue:(dispatch_queue_t)cltQueue;

-(void)delDelegateId:(id<MSClientDelegate>)cltDelegate;

-(int)connToServer:(NSString*)server
              port:(int)port;

-(MCConnState)connStatus;

@end

#endif /* MSClientManager_h */
