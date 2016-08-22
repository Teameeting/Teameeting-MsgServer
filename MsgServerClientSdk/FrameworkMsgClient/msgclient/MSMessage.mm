//
//  MSMessage.mm
//  SdkMsgClientIos
//
//  Created by hp on 6/29/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclient/MSMessage.h"

@implementation MSMessage

@synthesize content = _content;
@synthesize extra = _extra;
@synthesize msgId = _msgId;
@synthesize toId = _toId;
@synthesize fromId = _fromId;
@synthesize groupId = _groupId;
@synthesize nickName = _nickName;
@synthesize uiconUrl = _uiconUrl;
@synthesize cash = _cash;
@synthesize wishcont = _wishcont;
@synthesize millSec = _millSec;
@synthesize msgType = _msgType;
@synthesize flag = _flag;
@synthesize push = _push;
MJExtensionLogAllProperties

@end

