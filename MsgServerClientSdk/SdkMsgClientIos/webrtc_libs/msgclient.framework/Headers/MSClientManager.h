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
#import "msgclient/MSClientType.h"
#import "msgclient/MSClientDelegate.h"

@interface MSClientManager : NSObject

/**
 *  init msg client
 *
 *  params:
 *  usrId: the userid
 *  token: the user token
 *  nName: the user nick name
 *
 *  return:
 *      0: init ok
 *      -101: one of params are null
 */
-(int)initMsgClientUsrId:(NSString*)usrId
                   token:(NSString*)token
                   nName:(NSString*)nName;

/**
 *  uninit msg client
 *
 *  params:
 *
 *  return:
 *      0: unin ok
 */
-(int)uninMsgClient;

/**
 *  add client delegate
 *
 *  params:
 *      cltDelegate: the client delegate for callback to set
 *      cltQueue: the dispatch queue
 *
 *  return:
 *      void
 */
-(void)addDelegateId:(id<MSClientDelegate>)cltDelegate
       delegateQueue:(dispatch_queue_t)cltQueue;

/**
 *  remove client delegate
 *
 *  params:
 *      cltDelegate: the client delegate to delete
 *
 *  return:
 *      void
 */
-(void)delDelegateId:(id<MSClientDelegate>)cltDelegate;

/**
 *  connect to server
 *
 *  params:
 *      server: the msg server ip
 *      port: the msg server port
 *
 *  return:
 *      0: connect server ok
 *      -101: params is null
 */
-(int)connToServer:(NSString*)server
              port:(int)port;

/**
 *  get connection status
 *
 *  params:
 *
 *  return:
 *      MCConnState: the state of the connection to server
 */
-(MCConnState)connStatus;

/**
 *  set userid
 *
 *  params:
 *      userid: the userid to set
 *
 *  (but this func has not implement)
 *
 *  return:
 *      void
 *
 */
-(void)setUserId:(NSString*)userid;

/**
 *  set user token
 *
 *  params:
 *      token: the user token
 *
 *  return:
 *      void
 */
-(void)setToken:(NSString*)token;

/**
 *  set user nick name
 *
 *  params:
 *      nName: the user nick name
 *
 *  return:
 *      void
 */
-(void)setNickName:(NSString*)nName;

/**
 *  set user icon url
 *
 *  params:
 *      url: the user icon url
 *
 *  return:
 *      void
 */
-(void)setUIconUrl:(NSString*)url;

@end

#endif /* MSClientManager_h */
