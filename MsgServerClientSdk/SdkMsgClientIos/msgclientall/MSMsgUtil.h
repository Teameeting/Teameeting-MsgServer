//
//  MSMsgUtil.h
//  SdkMsgClientIos
//
//  Created by hp on 7/12/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#ifndef MSMsgUtil_h
#define MSMsgUtil_h
#import <Foundation/Foundation.h>
#import "MJExtension.h"
#import "MSTxtMessage.h"

@interface MSMsgUtil : NSObject

+ (NSString*)JSONToNSString:(id)obj;

+ (id)NSStringToJSONWithString:(NSString*)jstr;

/////////////////////////////////////////////////

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                  content:(NSString*)content;

+ (MSMessage*)EncodeMessageWithUserId:(NSString*)userId
                                 content:(NSString*)content;

+ (MSTxtMessage*)EncodeMessageWithUserIds:(NSArray*)array
                                  content:(NSString*)content;

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                   userId:(NSString*)userId;

+ (MSTxtMessage*)DecodeDictToMessageWithDict:(NSDictionary*)dict;

@end

#endif /* MSMsgUtil_h */
