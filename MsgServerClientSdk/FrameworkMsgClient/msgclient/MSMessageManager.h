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
 *      txtDelegate: the message delegate for callback to set
 *      txtQueue: the dispatch queue
 *
 *  return:
 *      void
 */
-(void)addDelegateId:(id<MSTxtMessageDelegate>)txtDelegate
                     delegateQueue:(dispatch_queue_t)txtQueue;

/**
 *  remove message delegate
 *
 *  params:
 *      txtDelegate: the message delegate to delete
 *
 *  return:
 *      void
 */
-(void)delDelegateId:(id<MSTxtMessageDelegate>)txtDelegate;

/**
 *  sync message when first login or refresh
 *  do not call this function in an loooooop
 *
 *  params:
 *
 *  return:
 *      void
 */
-(void)syncMsg;

/**
 *  you send message in grpId
 *
 *  params:
 *      grpId: the group id you are in
 *      content: the message you send
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: grpId or content is null
 */
-(int)sendTxtMsgGrpId:(NSString*)grpId
                 cont:(NSString*)content
               cmsgid:(NSString**)cmsgid;

/**
 *  you send content to usres in grpId
 *
 *  params:
 *      grpId: the group id you are in
 *      users: the users you will send to in grpId
 *      content: the message you send
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: grpId or users or content is null
 */
-(int)sendTxtMsgTosGrpId:(NSString*)grpId
                   users:(NSArray*)users
                    cont:(NSString*)content
                  cmsgid:(NSString**)cmsgid;

/**
 *  you send content to userId
 *
 *  params:
 *      usrId: the user id you will send to
 *      content: the message you send
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: usrId or content is null
 */
-(int)sendTxtMsgToUsrId:(NSString*)usrId
                   cont:(NSString*)content
                 cmsgid:(NSString**)cmsgid;

/**
 *  you send content to userIds
 *
 *  params:
 *      usrIds: the users ids you will send to
 *      content: the message you send
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: usrIds or content is null
 */
-(int)sendTxtMsgToUsrIds:(NSArray*)usrIds
                    cont:(NSString*)content
                  cmsgid:(NSString**)cmsgid;

/**
 *  hostId begin to live in grpId
 *
 *  params:
 *      grpId: the group id you are in
 *      hostId: the id of the host who is living, here means yourself id
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: grpId or hostId is null
 */
-(int)sendNotifyLiveGrpId:(NSString*)grpId
                   hostId:(NSString*)hostId
                   cmsgid:(NSString**)cmsgid;

/**
 *  you send red-envelope to hostId in grpId
 *
 *  params:
 *      grpId: the group id you are in
 *      hostId: the id of the host you will send to
 *      cash: the cash mount you will send to host
 *      cont: the wish message you want say to host
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: grpId or hostId or cash or cont is null
 */
-(int)sendNotifyRedEnvelopeGrpId:(NSString*)grpId
                          hostId:(NSString*)hostId
                            cash:(NSString*)cash
                            cont:(NSString*)cont
                          cmsgid:(NSString**)cmsgid;

/**
 *  userId was push to blacklist in grpId
 *
 *  params:
 *      grpId: the group id you are in
 *      userId: the user you want to pull blacklist
 *      notifys: the users you want to notify, want them to know, usually group managers or group owner
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: grpId or userId or notifys is null
 */
-(int)sendNotifyBlacklistGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                       notifys:(NSArray*)notifys
                        cmsgid:(NSString**)cmsgid;

/**
 *  userId was forbidden in grpId
 *
 *  params:
 *      grpId: the group id you are in
 *      userId: the user you want to forbidden talk in this group
 *      notifys: the users you want to notify, want them to know, usually group managers or group owner
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: grpId or userId or notifys is null
 */
-(int)sendNotifyForbiddenGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                       notifys:(NSArray*)notifys
                        cmsgid:(NSString**)cmsgid;

/**
 *  userId in grpId was setted to be manager, this message will notify all members
 *
 *  params:
 *      grpId: the group id you are in
 *      userId: the user you want to set to be manager
 *      cmsgid: the msgid of the message you send
 *      after invoke, you will get the id of this message
 *
 *  return:
 *      >0: the whole msg length
 *      -100: cmsgid is null
 *      -101: grpId or userId or notifys is null
 */
-(int)sendNotifySettedMgrGrpId:(NSString*)grpId
                        userId:(NSString*)userId
                        cmsgid:(NSString**)cmsgid;

@end

#endif /* MSMessageManager_h */
