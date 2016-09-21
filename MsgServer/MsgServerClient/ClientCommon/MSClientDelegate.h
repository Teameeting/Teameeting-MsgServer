//
//  MSClientDelegate.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSClientDelegate_h
#define MSClientDelegate_h


class MSClientDelegate {
public:
/**
 *  when server was connected this function will be called
 *
 *  params:
 */
    virtual void OnMsgServerConnected() = 0;

/**
 *  when server was connecting this function will be called
 *
 *  params:
 */
    virtual void OnMsgServerConnecting() = 0;

/**
 *  when server was disconnected this function will be called
 *
 *  params:
 */
    virtual void OnMsgServerDisconnect() = 0;

/**
 *  when server connect failed this function will be called
 *
 *  params:
 */
    virtual void OnMsgServerConnectionFailure() = 0;

/**
 * when client initializing this function will be called
 *
 *  params:
 */
    virtual void OnMsgClientInitializing() = 0;

/**
 *  when client initialize ok this function will be called
 *
 *  params:
 */
    virtual void OnMsgClientInitialized() = 0;

/**
 *  when client un initialize ok this function will be called
 *
 *  params:
 */
    virtual void OnMsgClientUnInitialize() = 0;

/**
 *  when client initialize failed this function will be called
 *
 *  params:
 */
    virtual void OnMsgClientInitializeFailure() = 0;

};

#endif /* MSClientDelegate_h */
