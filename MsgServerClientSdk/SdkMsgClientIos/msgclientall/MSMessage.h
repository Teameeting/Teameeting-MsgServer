//
//  MSMessage.h
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSMessage_h
#define MSMessage_h

#import <Foundation/Foundation.h>

@interface MSMessage : NSObject;

@property (nonatomic, strong) NSString* content;

@property (nonatomic, strong) NSString* extra;

@property (nonatomic, strong) NSString* msgId;

@property (nonatomic, strong) NSString* toId;

@property (nonatomic, strong) NSString* fromId;

@property (nonatomic, strong) NSString* groupId;

@property (nonatomic, strong) NSDate* time;

@property (nonatomic, strong) NSDate* date;

@property (nonatomic) unsigned int  millSec;//send second
@property (nonatomic) int           flag;//group or single
@property (nonatomic) int           tag;//txt or pic or self
@property (nonatomic) int           role;//sender or recver

@end

#endif /* MSMessage_h */
