//
//  MSClientManager.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSClientManager_h
#define MSClientManager_h

#include <string>
#include "MSClientType.h"
#include "MSClientDelegate.h"

class MSClientManager{
public:
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
    int InitMsgClient(const std::string& usrId, const std::string& token, const std::string& nName);

/**
 *  uninit msg client
 *
 *  params:
 *
 *  return:
 *      0: unin ok
 */
    int UninMsgClient();

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
    void AddDelegate(const MSClientDelegate* cltDelegate);

/**
 *  remove client delegate
 *
 *  params:
 *      cltDelegate: the client delegate to delete
 *
 *  return:
 *      void
 */
    void DelDelegate(const MSClientDelegate* cltDelegate);

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
    int ConnToServer(const std::string& serve, int port);

/**
 *  get connection status
 *
 *  params:
 *
 *  return:
 *      MCConnState: the state of the connection to server
 */
    MCConnState ConnStatus();

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
    void SetUserId(const std::string& userId);

/**
 *  set user token
 *
 *  params:
 *      token: the user token
 *
 *  return:
 *      void
 */
    void SetToken(const std::string& token);

/**
 *  set user nick name
 *
 *  params:
 *      nName: the user nick name
 *
 *  return:
 *      void
 */
    void SetNickName(const std::string& nName);

/**
 *  set user icon url
 *
 *  params:
 *      url: the user icon url
 *
 *  return:
 *      void
 */
    void SetUIconUrl(const std::string& url);

/**
 *  set if need push
 *
 *  params:
 *      push: 1, enable push, 2, disable push
 *
 *  return:
 *      void
 */
    void EnablePush(int push);

/**
 *  if mute notifications
 *
 *  params:
 *      mute: 1, mute notification, 2, not mute notification
 *
 *  return:
 *      void
 */
    void MuteNotify(int mute);

};

#endif /* MSClientManager_h */
