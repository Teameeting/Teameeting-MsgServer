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

/**
 *  add message delegate
 *
 *  params:
 */
-(void)addDelegateId:(id<MSTxtMessageDelegate>)txtDelegate
                     delegateQueue:(dispatch_queue_t)txtQueue;

/**
 *  remove message delegate
 *
 *  params:
 */
-(void)delDelegateId:(id<MSTxtMessageDelegate>)txtDelegate;

/**
 *  sync message when first login or refresh
 *  do not call this function in an loooooop
 *
 *  params:
 */
-(void)syncMsg;

/**
 *  you send content in grpId
 *
 *  params:
 */
-(int)sendTxtMsgGrpId:(NSString*)grpId
                 cont:(NSString*)content
               cmsgid:(NSString**)cmsgid;

/**
 *  you send content to userId
 *
 *  params:
 */
-(int)sendTxtMsgToUsrId:(NSString*)usrId
                   cont:(NSString*)content
                 cmsgid:(NSString**)cmsgid;

/**
 *  you send content to userIds
 *
 *  params:
 */
-(int)sendTxtMsgToUsrIds:(NSArray*)usrIds
                    cont:(NSString*)content
                  cmsgid:(NSString**)cmsgid;

/**
 *  hostId begin to live in grpId
 *
 *  params:
 */
-(int)sendNotifyLiveGrpId:(NSString*)grpId
                   hostId:(NSString*)hostId
                   cmsgid:(NSString**)cmsgid;

/**
 *  you send red-envelope to hostId in grpId
 *
 *  params:
 */
-(int)sendNotifyRedEnvelopeGrpId:(NSString*)grpId
                          hostId:(NSString*)hostId
                          cmsgid:(NSString**)cmsgid;

/**
 *  userId was push to blacklist in grpId
 *
 *  params:
 */
-(int)sendNotifyBlacklistGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString**)cmsgid;

/**
 *  userId was forbidden in grpId
 *
 *  params:
 */
-(int)sendNotifyForbiddenGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString**)cmsgid;

/**
 *  userId in grpId was setted to be manager
 *
 *  params:
 */
-(int)sendNotifySettedMgrGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString**)cmsgid;

@end

#endif /* MSMessageManager_h */
