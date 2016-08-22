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

+ (MSMessage*)EncodeMessageWithTxtMsg:(MSTxtMessage*)txtMsg
                                  msgType:(MCMsgType)msgType
{
    MSMessage *mMsg = [[MSMessage alloc] init];
    [mMsg setContent:[txtMsg getContent]];
    [mMsg setGroupId:[txtMsg getGroupId]];
    [mMsg setPush:[txtMsg getPush]];
    [mMsg setToId:[txtMsg getToId]];
    [mMsg setNickName:MsgClient::Instance().MCGetNsNickName()];
    [mMsg setUiconUrl:MsgClient::Instance().MCGetNsUIconUrl()];
    [mMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [mMsg setMsgType:msgType];
    return mMsg;
}

+ (MSMessage*)EncodeMessageWithLivMsg:(MSLivMessage*)livMsg
                              msgType:(MCMsgType)msgType
{
    MSMessage *mMsg = [[MSMessage alloc] init];
    [mMsg setGroupId:[livMsg getGroupId]];
    [mMsg setToId:[livMsg getToId]];
    [mMsg setPush:[livMsg getPush]];
    [mMsg setNickName:MsgClient::Instance().MCGetNsNickName()];
    [mMsg setUiconUrl:MsgClient::Instance().MCGetNsUIconUrl()];
    [mMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [mMsg setFlag:[livMsg getFlag]];
    [mMsg setMsgType:msgType];
    return mMsg;
}

+ (MSMessage*)EncodeMessageWithRenMsg:(MSRenMessage*)renMsg
                              msgType:(MCMsgType)msgType
{
    MSMessage *mMsg = [[MSMessage alloc] init];
    [mMsg setGroupId:[renMsg getGroupId]];
    [mMsg setToId:[renMsg getToId]];
    [mMsg setPush:[renMsg getPush]];
    [mMsg setNickName:MsgClient::Instance().MCGetNsNickName()];
    [mMsg setUiconUrl:MsgClient::Instance().MCGetNsUIconUrl()];
    [mMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [mMsg setCash:[renMsg getCash]];
    [mMsg setWishcont:[renMsg getWishcont]];
    [mMsg setMsgType:msgType];
    return mMsg;
}

+ (MSMessage*)EncodeMessageWithBlkMsg:(MSBlkMessage*)blkMsg
                              msgType:(MCMsgType)msgType
{
    MSMessage *mMsg = [[MSMessage alloc] init];
    [mMsg setGroupId:[blkMsg getGroupId]];
    [mMsg setToId:[blkMsg getToId]];
    [mMsg setPush:[blkMsg getPush]];
    [mMsg setNickName:MsgClient::Instance().MCGetNsNickName()];
    [mMsg setUiconUrl:MsgClient::Instance().MCGetNsUIconUrl()];
    [mMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [mMsg setFlag:[blkMsg getFlag]];
    [mMsg setMsgType:msgType];
    return mMsg;
}

+ (MSMessage*)EncodeMessageWithFbdMsg:(MSFbdMessage*)fbdMsg
                              msgType:(MCMsgType)msgType
{
    MSMessage *mMsg = [[MSMessage alloc] init];
    [mMsg setGroupId:[fbdMsg getGroupId]];
    [mMsg setToId:[fbdMsg getToId]];
    [mMsg setPush:[fbdMsg getPush]];
    [mMsg setNickName:MsgClient::Instance().MCGetNsNickName()];
    [mMsg setUiconUrl:MsgClient::Instance().MCGetNsUIconUrl()];
    [mMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [mMsg setFlag:[fbdMsg getFlag]];
    [mMsg setMsgType:msgType];
    return mMsg;
}

+ (MSMessage*)EncodeMessageWithMgrMsg:(MSMgrMessage*)mgrMsg
                                  msgType:(MCMsgType)msgType
{
    MSMessage *mMsg = [[MSMessage alloc] init];
    [mMsg setGroupId:[mgrMsg getGroupId]];
    [mMsg setToId:[mgrMsg getToId]];
    [mMsg setPush:[mgrMsg getPush]];
    [mMsg setNickName:MsgClient::Instance().MCGetNsNickName()];
    [mMsg setUiconUrl:MsgClient::Instance().MCGetNsUIconUrl()];
    [mMsg setFromId:MsgClient::Instance().MCGetNsUserId()];
    [mMsg setFlag:[mgrMsg getFlag]];
    [mMsg setMsgType:msgType];
    return mMsg;
}

/////////////////////////////////////////////////

+ (MSMessage*)DecodeDictToMessageWithDict:(NSDictionary*)dict
{
    MSMessage *mMsg = [MSMessage mj_objectWithKeyValues:dict];
    return mMsg;
}

@end
