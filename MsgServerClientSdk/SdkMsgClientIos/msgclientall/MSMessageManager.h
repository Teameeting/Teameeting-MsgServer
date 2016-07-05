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
-(int)sendTxtMsgGrpId:(NSString*)grpId
                 cont:(NSString*)content
               cmsgid:(NSString**)cmsgid;

// you send content to userId
-(int)sendTxtMsgToUsrId:(NSString*)usrId
                   cont:(NSString*)content
                 cmsgid:(NSString**)cmsgid;

// you send content to userIds
-(int)sendTxtMsgToUsrIds:(NSArray*)usrIds
                    cont:(NSString*)content
                  cmsgid:(NSString**)cmsgid;

// hostId begin to live in grpId
-(int)sendNotifyLiveGrpId:(NSString*)grpId
                   hostId:(NSString*)hostId
                   cmsgid:(NSString**)cmsgid;

// you send red-envelope to hostId in grpId
-(int)sendNotifyRedEnvelopeGrpId:(NSString*)grpId
                          hostId:(NSString*)hostId
                          cmsgid:(NSString**)cmsgid;

// userId was push to blacklist in grpId
-(int)sendNotifyBlacklistGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString**)cmsgid;

// userId was forbidden in grpId
-(int)sendNotifyForbiddenGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString**)cmsgid;

// userId in grpId was setted to be manager
-(int)sendNotifySettedMgrGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString**)cmsgid;

@end

#endif /* MSMessageManager_h */
