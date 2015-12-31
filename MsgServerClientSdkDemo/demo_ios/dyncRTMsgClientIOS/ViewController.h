//
//  ViewController.h
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MsgClientProtocolImpl.h"
@class TMMsgSender;

@interface ViewController : UIViewController<ShowResultProtocol>{
    TMMsgSender *msgsender;
    MsgClientProtocolImpl *impl;
    int         port;
    NSString    *server;
    NSString    *userid;
    NSString    *pass;
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



