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
#import "MSSubMessageDelegate.h"
#import "MSSubMessage.h"

@interface MSMessageManager : NSObject

/**
 *  add message delegate
 *
 *  params:
 *      subDelegate: the message delegate for callback to set
 *      subQueue: the dispatch queue
 *
 *  return:
 *      void
 */
-(void)addDelegateId:(id<MSSubMessageDelegate>)subDelegate
                     delegateQueue:(dispatch_queue_t)subQueue;

/**
 *  remove message delegate
 *
 *  params:
 *      subDelegate: the message delegate to delete
 *
 *  return:
 *      void
 */
-(void)delDelegateId:(id<MSSubMessageDelegate>)subDelegate;

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
 *  this will sync msg to db
 *  when app will exit or crash ot leave view
 *  MUST invoke this to save data
 *
 *  params:
 *
 *  return:
 *      void
 */
-(void)sync2Db;

/**
 *  you send message in grpId
 *
 *  params:
 *      txtMsg: the message you will send
 *      GroupId and Content and Push need to be setted
 *      GroupId: the group id you are in
 *      Content: the content you send
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *
 *  return:
 *      msgid: the id of this msg
 */
-(NSString*)sendTxtMsg:(MSTxtMessage*)txtMsg;

/**
 *  you send content to usres in grpId
 *
 *  params:
 *      txtMsg: the message you will send
 *      GroupId and Content and Push need to be setted
 *      GroupId: the group id you are in
 *      Content: the message you send
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *
 *  return:
 *      msgid: the id of this msg
 */
-(NSString*)sendTxtMsgTos:(MSTxtMessage*)txtMsg
                    users:(NSArray*)users;

/**
 *  you send content to userId
 *
 *  params:
 *      txtMsg: the message you will send
 *      Content and ToId and Push need to be setted
 *      Content: the message you send
 *      ToId: the user id you will send to
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *
 *  return:
 *      msgid: the id of this msg
 */
-(NSString*)sendTxtMsgToUser:(MSTxtMessage*)txtMsg;

/**
 *  you send content to userIds
 *
 *  params:
 *      txtMsg: the message you will send
 *      Content and Push need to be setted
 *      Content: the message you send
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *      users: the users ids you will send to
 *
 *  return:
 *      msgid: the id of this msg
 */

-(NSString*)sendTxtMsgToUsers:(MSTxtMessage*)txtMsg
                        users:(NSArray*)users;
/**
 *  hostId begin to live in grpId
 *
 *  params:
 *      txtMsg: the message you will send
 *      GroupId and ToId and Flag and Push need to be setted
 *      GroupId: the group id you are in
 *      ToId: the user you want to forbidden talk in this group
 *      Flag: 1, forbidden user, 0, cancel forbidden
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *
 *  return:
 *      msgid: the id of this msg
 */
-(NSString*)sendNotifyLive:(MSLivMessage*)livMsg;

/**
 *  you send red-envelope to hostId in grpId
 *
 *  params:
 *      txtMsg: the message you will send
 *      GroupId and ToId and and Push and Cash and Wishcont need to be setted
 *      GroupId: the group id you are in
 *      ToId: the user you want to forbidden talk in this group
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *      Cash: the cash mount you will send to host
 *      Wishcont: the wish message you want say to host
 *
 *  return:
 *      msgid: the id of this msg
 */
-(NSString*)sendNotifyRedEnvelope:(MSRenMessage*)renMsg;

/**
 *  userId was push to blacklist in grpId
 *
 *  params:
 *      txtMsg: the message you will send
 *      GroupId and ToId and Flag and Push need to be setted
 *      GroupId: the group id you are in
 *      ToId: the user you want to forbidden talk in this group
 *      Flag: 1, forbidden user, 0, cancel forbidden
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *      notifys: the users you want to notify, want them to know, usually group managers or group owner
 *
 *  return:
 *      msgid: the id of this msg
 */
-(NSString*)sendNotifyBlacklist:(MSBlkMessage*)blkMsg
                        notifys:(NSArray*)notifys;

/**
 *  userId was forbidden in grpId
 *
 *  params:
 *      txtMsg: the message you will send
 *      GroupId and ToId and Flag and Push need to be setted
 *      GroupId: the group id you are in
 *      ToId: the user you want to forbidden talk in this group
 *      Flag: 1, forbidden user, 0, cancel forbidden
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *      notifys: the users you want to notify, want them to know, usually group managers or group owner
 *
 *  return:
 *      msgid: the id of this msg
 */
-(NSString*)sendNotifyForbidden:(MSFbdMessage*)fbdMsg
                        notifys:(NSArray*)notifys;

/**
 *  userId in grpId was setted to be manager, this message will notify all members
 *
 *  params:
 *      txtMsg: the message you will send
 *      GroupId and ToId and Flag and Push need to be setted
 *      GroupId: the group id you are in
 *      ToId: the user you want to set to be manager
 *      Flag: 1, set user mgr, 0, set user not mgr
 *      Push: 1, the msg will be pushed if offline, 0, not push
 *
 *  return:
 *      msgid: the id of this msg
 */
-(NSString*)sendNotifySettedMgr:(MSMgrMessage*)mgrMsg;

@end

#endif /* MSMessageManager_h */
