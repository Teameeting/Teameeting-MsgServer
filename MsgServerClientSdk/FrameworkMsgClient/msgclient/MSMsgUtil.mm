//
//  MSMsgUtil.m
//  SdkMsgClientIos
//
//  Created by hp on 7/12/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "msgclient/MSMsgUtil.h"
#import "msgclient/MsgClient.h"

@implementation MSMsgUtil

+ (NSString*)JSONToNSString:(id)obj
{
    NSString *jsonString = nil;
    NSError *error = nil;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:obj
                                                       options:NSJSONWritingPrettyPrinted // Pass 0 if you don't care about the readability of the generated string
                                                         error:&error];
    if (!jsonData) {
        NSLog(@"MSMsgUtil JSONToString Got an error: %@", error);
    } else {
        jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
    }
    return jsonString;
}

+ (id)NSStringToJSONWithString:(NSString*)jstr
{
    NSError *error = nil;
    NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:[jstr dataUsingEncoding:NSUTF8StringEncoding] options:NSJSONReadingMutableLeaves error:&error];
    if (error) {
        NSLog(@"NSStringToJSONWithString error:%@", error);
    }
    return dict;
}

/////////////////////////////////////////////////

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                  content:(NSString*)content
                                  msgType:(MCMsgType)msgType
{
    MSTxtMessage *txtMsg = [[MSTxtMessage alloc] init];
    
    [txtMsg setContent:content];
    [txtMsg setGroupId:groupId];
    [txtMsg setNickname:MsgClient::Instance().MCGetNsNickName()];
    [txtMsg setUiconurl:MsgClient::Instance().MCGetNsUIconUrl()];
    [txtMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [txtMsg setMsgType:msgType];
    
    return txtMsg;
}

+ (MSTxtMessage*)EncodeMessageWithUserId:(NSString*)userId
                                 content:(NSString*)content
                                 msgType:(MCMsgType)msgType
{
    MSTxtMessage *txtMsg = nil;
    txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setContent:content];
    [txtMsg setToId:userId];
    [txtMsg setNickname:MsgClient::Instance().MCGetNsNickName()];
    [txtMsg setUiconurl:MsgClient::Instance().MCGetNsUIconUrl()];
    [txtMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [txtMsg setMsgType:msgType];
    return txtMsg;
}

+ (MSTxtMessage*)EncodeMessageWithUserIds:(NSArray*)array
                                  content:(NSString*)content
                                  msgType:(MCMsgType)msgType
{
    MSTxtMessage *txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setContent:content];
    [txtMsg setNickname:MsgClient::Instance().MCGetNsNickName()];
    [txtMsg setUiconurl:MsgClient::Instance().MCGetNsUIconUrl()];
    [txtMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [txtMsg setMsgType:msgType];
    return txtMsg;
}

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                   userId:(NSString*)userId
                                  msgType:(MCMsgType)msgType
{
    MSTxtMessage *txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setGroupId:groupId];
    [txtMsg setToId:userId];
    [txtMsg setNickname:MsgClient::Instance().MCGetNsNickName()];
    [txtMsg setUiconurl:MsgClient::Instance().MCGetNsUIconUrl()];
    [txtMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [txtMsg setMsgType:msgType];
    return txtMsg;
}

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                   userId:(NSString*)userId
                                     cash:(NSString *)cash
                                     cont:(NSString *)cont
                                  msgType:(MCMsgType)msgType
{
    MSTxtMessage *txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setGroupId:groupId];
    [txtMsg setToId:userId];
    [txtMsg setNickname:MsgClient::Instance().MCGetNsNickName()];
    [txtMsg setUiconurl:MsgClient::Instance().MCGetNsUIconUrl()];
    [txtMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [txtMsg setCash:cash];
    [txtMsg setWishcont:cont];
    [txtMsg setMsgType:msgType];
    return txtMsg;
}

+ (MSTxtMessage*)EncodeMessageWithGroupId:(NSString*)groupId
                                  msgType:(MCMsgType)msgType
{
    MSTxtMessage *txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setGroupId:groupId];
    [txtMsg setNickname:MsgClient::Instance().MCGetNsNickName()];
    [txtMsg setUiconurl:MsgClient::Instance().MCGetNsUIconUrl()];
    [txtMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [txtMsg setMsgType:msgType];
    return txtMsg;
}

/////////////////////////////////////////////////

+ (MSTxtMessage*)DecodeDictToMessageWithDict:(NSDictionary*)dict
{
    MSTxtMessage *txtMsg = [MSTxtMessage mj_objectWithKeyValues:dict];
    return txtMsg;
}

@end
