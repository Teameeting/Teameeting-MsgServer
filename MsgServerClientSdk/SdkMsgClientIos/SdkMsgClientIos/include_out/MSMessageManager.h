//
//  MSMessageManager.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSMessageManager_h
#define MSMessageManager_h
#import <Foundation/Foundation.h>
#import "MSTxtMessageDelegate.h"

@interface MSMessageManager : NSObject

-(void)addDelegateId:(id<MSTxtMessageDelegate>)txtDelegate
                     delegateQueue:(dispatch_queue_t)txtQueue;

-(void)delDelegateId:(id<MSTxtMessageDelegate>)txtDelegate;

// sync message when first login or refresh
// do not call this function in an loooooop
-(void)syncMsg;

// you send content in grpId
-(void)sendTxtMsgGrpId:(NSString*)grpId
                  cont:(NSString*)content;

// you send content to userId
-(void)sendTxtMsgToUsrId:(NSString*)usrId
                    cont:(NSString*)content;

// you send content to userIds
-(void)sendTxtMsgToUsrIds:(NSArray*)usrIds
                     cont:(NSString*)content;

// hostId begin to live in grpId
-(void)sendNotifyLiveGrpId:(NSString*)grpId
                    hostId:(NSString*)hostId;

// you send red-envelope to hostId in grpId
-(void)sendNotifyRedEnvelopeGrpId:(NSString*)grpId
                           hostId:(NSString*)hostId;

// userId was push to blacklist in grpId
-(void)sendNotifyBlacklistGrpId:(NSString*)grpId
                         userId:(NSString*)userId;

// userId was forbidden in grpId
-(void)sendNotifyForbiddenGrpId:(NSString*)grpId
                         userId:(NSString*)userId;

// userId in grpId was setted to be manager
-(void)sendNotifySettedMgrGrpId:(NSString*)grpId
                         userId:(NSString*)userId;

@end

#endif /* MSMessageManager_h */
