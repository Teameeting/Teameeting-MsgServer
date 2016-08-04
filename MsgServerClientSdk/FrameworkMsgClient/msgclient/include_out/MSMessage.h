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
#import "MJExtension.h"

@interface MSMessage : NSObject;

@property (nonatomic, strong) NSString* content;

@property (nonatomic, strong) NSString* extra;

@property (nonatomic, strong) NSString* msgId;

@property (nonatomic, strong) NSString* toId;

@property (nonatomic, strong) NSString* fromId;

@property (nonatomic, strong) NSString* groupId;

@property (nonatomic, strong) NSString* nickname;

@property (nonatomic, strong) NSString* uiconurl;

@property (nonatomic, strong) NSString* cash;

@property (nonatomic, strong) NSString* wishcont;

@property (nonatomic) unsigned int  millSec;//send second

@end

#endif /* MSMessage_h */
