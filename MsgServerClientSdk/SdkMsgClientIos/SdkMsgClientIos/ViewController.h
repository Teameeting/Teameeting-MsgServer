//
//  ViewController.h
//  SdkMsgClientIos
//
//  Created by hp on 1/31/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MsgClientProtocolImpl.h"
@class MSClientManager;
@class MSGroupManager;
@class MSMessageManager;

@interface ViewController : UIViewController<ShowResultProtocol>{
    MSClientManager *clientMgr;
    MSGroupManager *groupMgr;
    MSMessageManager *msgMgr;
    MsgClientProtocolImpl *impl;
    int         port;
    NSString    *server;
    NSString    *uid;
    NSString    *token;
    NSString    *nname;
    NSString    *roomid;
    NSString    *msg;
}

-(void)updateResult:(NSString*)backInfo
               mMsg:(NSString*)mMsg
               nInt:(int)nInt;

-(void)resultDisplayCallback:(NSString*)backInfo
                        mMsg:(NSString*)mMsg
                        nInt:(int)nInt;
@end

