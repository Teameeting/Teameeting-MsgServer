//
//  MSMessage.mm
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclientall/MSMessage.h"
#import "MJExtension.h"
@implementation MSMessage

@synthesize content = _content;
@synthesize extra = _extra;
@synthesize msgId = _msgId;
@synthesize toId = _toId;
@synthesize fromId = _fromId;
@synthesize groupId = _groupId;
@synthesize nickname = _nickname;
@synthesize uiconurl = _uiconurl;
@synthesize time = _time;
@synthesize date = _date;
@synthesize millSec = _millSec;
@synthesize flag = _flag;
@synthesize tag = _tag;
@synthesize role = _role;
MJExtensionLogAllProperties
@end

