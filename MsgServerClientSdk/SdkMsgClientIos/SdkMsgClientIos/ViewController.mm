//
//  ViewController.m
//  SdkMsgClientIos
//
//  Created by hp on 1/31/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <AdSupport/ASIdentifierManager.h>
#import "ViewController.h"
#import <msgclient/MSClientManager.h>
#import <msgclient/MSGroupManager.h>
#import <msgclient/MSMessageManager.h>


#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

@interface ViewController () {
    UIButton *mEnterButton;
    UIButton *mLeaveButton;
    UIButton *mSndMsgButton;
    UIButton *mSndMsgTosButton;
    UIButton *mSndMsgToUserButton;
    UIButton *mSndMsgToUsersButton;
    UIButton *mNotifyLiveButton;
    UIButton *mNotifyRedEnvelopeButton;
    UIButton *mNotifyBlacklistButton;
    UIButton *mNotifyForbiddenButton;
    UIButton *mNotifySettedMgrButton;
    UITextView *textViewDisplay;
    UIButton *mSettingPush;
    UIButton *mSettingMuteNotify;
    NSString *mGroupId;
}

@end

@implementation ViewController

- (void)SetButtonParamsButton:(UIButton*)uibutton
                        title:(NSString*)title
                        state:(NSString*)state
{
    assert(uibutton!=nullptr);
    [uibutton setTitle:title forState:UIControlStateNormal];
    [uibutton setTitle:state forState:UIControlStateHighlighted];
    [uibutton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [uibutton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [uibutton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    uibutton.backgroundColor = [UIColor redColor];
}

-(void) packString:(NSString*)message
{
    
    NSUInteger numberOfBytes = [message lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
    NSLog(@"message length:%u, numberOfBytes:%u", [message length], numberOfBytes);
    void *buffer = malloc(numberOfBytes);
    if (!buffer) return;
    NSUInteger usedLength = 0;
    NSRange range = NSMakeRange(0, [message length]);
    BOOL result = [message getBytes:buffer maxLength:numberOfBytes usedLength:&usedLength encoding:NSUTF8StringEncoding options:0 range:range remainingRange:NULL];
    NSLog(@"result is:%d", result);
    Byte header[2] = {};
    header[0] = 0x04;
    header[1] = numberOfBytes;
    NSUInteger alllen = sizeof(header)+usedLength;
    Byte *all = (Byte*)malloc(alllen);
    if (!all)
    {
        if (buffer) free(buffer);
        return;
    }
    memcpy(all, header, sizeof(header));
    memcpy(all+sizeof(header), buffer, usedLength);
    
    NSData *nsd = [[NSData alloc] initWithBytes:all length:alllen];
    NSString *nss = [[NSString alloc] initWithData:nsd encoding:NSUTF8StringEncoding];
    NSLog(@"nss is:%@", nss);
    
    free(buffer);
    free(all);
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    [self packString:@"123456789"];
    
    mEnterButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 100, 200, 30)];
    [self SetButtonParamsButton:mEnterButton title:@"Enter" state:@"Entering..."];
    [mEnterButton addTarget:self action:@selector(enterButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mEnterButton];
    
    mLeaveButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 150, 200, 30)];
    [self SetButtonParamsButton:mLeaveButton title:@"Leave" state:@"Leaving..."];
    [mLeaveButton addTarget:self action:@selector(leaveButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mLeaveButton];
    
    /////////////////////////////////////////////////////
    
    mSndMsgButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 200, 200, 30)];
    [self SetButtonParamsButton:mSndMsgButton title:@"SndMsg" state:@"SndMsging..."];
    [mSndMsgButton addTarget:self action:@selector(sndMsgButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mSndMsgButton];
    
    mSndMsgTosButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 250, 200, 30)];
    [self SetButtonParamsButton:mSndMsgTosButton title:@"SndMsgTos" state:@"SndMsgTosing..."];
    [mSndMsgTosButton addTarget:self action:@selector(sndMsgTosButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mSndMsgTosButton];
    
    mSndMsgToUserButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 300, 200, 30)];
    [self SetButtonParamsButton:mSndMsgToUserButton title:@"SndMsgToUser" state:@"SndMsgToUsering..."];
    [mSndMsgToUserButton addTarget:self action:@selector(sndMsgToUserButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mSndMsgToUserButton];
    
    mSndMsgToUsersButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 350, 200, 30)];
    [self SetButtonParamsButton:mSndMsgToUsersButton title:@"SndMsgToUsers" state:@"SndMsgToUsersing..."];
    [mSndMsgToUsersButton addTarget:self action:@selector(sndMsgToUsersButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mSndMsgToUsersButton];
    
    mNotifyLiveButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 400, 200, 30)];
    [self SetButtonParamsButton:mNotifyLiveButton title:@"NotifyLive" state:@"NotifyLiving..."];
    [mNotifyLiveButton addTarget:self action:@selector(notifyLiveButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mNotifyLiveButton];
    
    mNotifyRedEnvelopeButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 450, 200, 30)];
    [self SetButtonParamsButton:mNotifyRedEnvelopeButton title:@"NotifyRedEnvelope" state:@"NotifyRedEnveloping..."];
    [mNotifyRedEnvelopeButton addTarget:self action:@selector(notifyRedEnvelopeButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mNotifyRedEnvelopeButton];
    
    mNotifyBlacklistButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 500, 200, 30)];
    [self SetButtonParamsButton:mNotifyBlacklistButton title:@"NotifyBlacklist" state:@"NotifyBlacklisting..."];
    [mNotifyBlacklistButton addTarget:self action:@selector(notifyBlacklistButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mNotifyBlacklistButton];
    
    mNotifyForbiddenButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 550, 200, 30)];
    [self SetButtonParamsButton:mNotifyForbiddenButton title:@"NotifyForbidden" state:@"NotifyForbiddening..."];
    [mNotifyForbiddenButton addTarget:self action:@selector(notifyForbiddenButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mNotifyForbiddenButton];
    
    mNotifySettedMgrButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 600, 200, 30)];
    [self SetButtonParamsButton:mNotifySettedMgrButton title:@"NotifySettedMgr" state:@"NotifySettedMgring..."];
    [mNotifySettedMgrButton addTarget:self action:@selector(notifySettedMgrButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mNotifySettedMgrButton];
    
    textViewDisplay = [[UITextView alloc] initWithFrame:CGRectMake(250, 100, 300, 900)];
    textViewDisplay.backgroundColor=[UIColor whiteColor];
    textViewDisplay.scrollEnabled = YES;
    textViewDisplay.editable = NO;
    textViewDisplay.font=[UIFont fontWithName:@"Arial" size:18.0];
    textViewDisplay.returnKeyType = UIReturnKeyDefault;
    textViewDisplay.keyboardType = UIKeyboardTypeDefault;
    textViewDisplay.textAlignment = NSTextAlignmentLeft;
    textViewDisplay.dataDetectorTypes = UIDataDetectorTypeAll;
    textViewDisplay.textColor = [UIColor blackColor];
    textViewDisplay.text = @"Result";
    [self.view addSubview:textViewDisplay];
    
    mSettingPush = [[UIButton alloc] initWithFrame:CGRectMake(250, 300, 200, 30)];
    [self SetButtonParamsButton:mSettingPush title:@"SettingPush" state:@"SettingPushing..."];
    [mSettingPush addTarget:self action:@selector(settingPushButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mSettingPush];
   
    mSettingMuteNotify = [[UIButton alloc] initWithFrame:CGRectMake(250, 350, 200, 30)];
    [self SetButtonParamsButton:mSettingMuteNotify title:@"SettingMuteNotify" state:@"SettingMuteNotify..."];
    [mSettingMuteNotify addTarget:self action:@selector(settingMuteButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mSettingMuteNotify];
    
    ////////////////////////////////////////////////////
    clientMgr = [[MSClientManager alloc] init];
    groupMgr = [[MSGroupManager alloc] init];
    msgMgr = [[MSMessageManager alloc] init];
    impl = [[MsgClientProtocolImpl alloc] init];
    impl.delegate = self;
    
    
    server = @"hui.msg.anyrtc.io";//@"192.168.7.207";
    port   = 6630;
    uid = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
    NSLog(@"this device uid is:%@", uid);
    token = @"ff6fda83c4331774e99eaf3553d68d21771efb344811e08fc649bdbbb10d449d";
    nname = @"nickname";
    roomid = @"400000000440";
    msg = @"hello world";
    
    int ret = [clientMgr initMsgClientUsrId:uid token:token nName:nname];
    if (ret != 0) {
        NSLog(@"clientMgr Init error, return -1!!!");
    }
    [msgMgr addDelegateId:impl delegateQueue:nil];
    [groupMgr addDelegateId:impl delegateQueue:nil];
    [clientMgr addDelegateId:impl delegateQueue:nil];
    [clientMgr connToServer:server port:port];
    [clientMgr setNickName:@"hehe"];
    [clientMgr setToken:@"ef2e594e1b2cbe75daebf205f2b2d0e2805ec51a6fc254b9b01d89a0dfd6015e"];
    [clientMgr setUIconUrl:@"hehehehe"];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)enterButton:(id)sender {
    NSString *enter = @"enterButton was called";
    NSLog(@"%@", enter);
    [msgMgr sync2Db];
    
    mGroupId = @"wocaowocaowocao";
    NSLog(@"====>>you enter mGroupId %@", mGroupId);
    [groupMgr addGroupGrpId:mGroupId];
}

- (IBAction)leaveButton:(id)sender {
    NSString *leave = @"leaveButton was called";
    NSLog(@"%@", leave);
    
    mGroupId = @"wocaowocaowocao";
    NSLog(@"====>>you leave mGroupId %@", mGroupId);
    [groupMgr rmvGroupGrpId:mGroupId];
}

- (IBAction)sndMsgButton:(id)sender {
    NSString *sndMsg = @"sndMsgButton was called";
    NSLog(@"%@", sndMsg);
    
    mGroupId = @"wocaowocaowocao";
    MSTxtMessage* txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setGroupId:mGroupId];
    [txtMsg setContent:sndMsg];
    [txtMsg setPush:0];
    NSLog(@"111 send grpid:%@, content:%@", [txtMsg getGroupId], [txtMsg getContent]);
    NSString *outmsgid = [msgMgr sendTxtMsg:txtMsg];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendTxtMsg outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendTxtMsg outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)sndMsgTosButton:(id)sender {
    NSString *sndMsgTos = @"sndMsgTosButton was called";
    NSLog(@"%@", sndMsgTos);
    
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *touser2 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSArray* arr = [NSArray arrayWithObjects:touser1, touser2, nil];
    mGroupId = @"wocaowocaowocao";
    MSTxtMessage *txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setContent:sndMsgTos];
    [txtMsg setGroupId:mGroupId];
    [txtMsg setPush:0];
    NSString *outmsgid = [msgMgr sendTxtMsgTos:txtMsg users:arr];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendTxtMsgTos outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendTxtMsgTos outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)sndMsgToUserButton:(id)sender {
    NSString *sndMsgToUser = @"sndMsgToUserButton was called";
    NSLog(@"%@", sndMsgToUser);
    
    NSString *touser = [NSString stringWithCString:"8ca64d158a505876" encoding:NSUTF8StringEncoding];
    MSTxtMessage *txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setContent:sndMsgToUser];
    [txtMsg setToId:touser];
    [txtMsg setPush:0];
    NSString *outmsgid = [msgMgr sendTxtMsgToUser:txtMsg];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendTxtMsgToUser outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendTxtMsgToUser outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)sndMsgToUsersButton:(id)sender {
    NSString *sndMsgToUsers = @"sndMsgToUsersButton was called";
    NSLog(@"%@", sndMsgToUsers);
    
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *touser2 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSArray *arr = [NSArray arrayWithObjects:touser1, touser2, nil];
    MSTxtMessage *txtMsg = [[MSTxtMessage alloc] init];
    [txtMsg setContent:sndMsgToUsers];
    [txtMsg setPush:0];
    NSString *outmsgid = [msgMgr sendTxtMsgToUsers:txtMsg users:arr];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendTxtMsgToUsers outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendTxtMsgToUsers outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifyLiveButton:(id)sender {
    NSString *notifyLive = @"notifyLiveButton was called";
    NSLog(@"%@", notifyLive);
    
    mGroupId = @"wocaowocaowocao";
    NSString *touser = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    MSLivMessage *livMsg = [[MSLivMessage alloc] init];
    [livMsg setGroupId:mGroupId];
    [livMsg setToId:touser];
    [livMsg setFlag:1];
    [livMsg setPush:1];
    NSString *outmsgid = [msgMgr sendNotifyLive:livMsg];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendLivMsg outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendLivMsg outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifyRedEnvelopeButton:(id)sender {
    NSString *notifyRedEnvelope = @"notifyRedEnvelopeButton was called";
    NSLog(@"%@", notifyRedEnvelope);
    
    mGroupId = @"wocaowocaowocao";
    NSString *touser = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *cash = @"123.45";
    NSString *wishcont = @"happy to you";
    MSRenMessage *renMsg = [[MSRenMessage alloc] init];
    [renMsg setGroupId:mGroupId];
    [renMsg setToId:touser];
    [renMsg setCash:cash];
    [renMsg setWishcont:wishcont];
    [renMsg setPush:1];
    NSString *outmsgid = [msgMgr sendNotifyRedEnvelope:renMsg];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendRenMsg outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendRenMsg outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifyBlacklistButton:(id)sender {
    NSString *notifyBlacklist = @"notifyBlacklistButton was called";
    NSLog(@"%@", notifyBlacklist);
    
    mGroupId = @"wocaowocaowocao";
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    //NSString *touser2 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    //NSArray *arr = [NSArray arrayWithObjects:touser1, touser2, nil];
    MSBlkMessage *blkMsg = [[MSBlkMessage alloc] init];
    [blkMsg setGroupId:mGroupId];
    [blkMsg setToId:touser1];
    [blkMsg setFlag:1];
    [blkMsg setPush:1];
    NSString *outmsgid = [msgMgr sendNotifyBlacklist:blkMsg notifys:nil];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendBlkMsg outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendBlkMsg outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifyForbiddenButton:(id)sender {
    NSString *notifyForbidden = @"notifyForbiddenButton was called";
    NSLog(@"%@", notifyForbidden);
    
    mGroupId = @"wocaowocaowocao";
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *touser2 = [NSString stringWithCString:"8ca64d158a505876" encoding:NSUTF8StringEncoding];
    NSArray *arr = [NSArray arrayWithObjects:touser1, touser2, nil];
    
    MSFbdMessage *fbdMsg = [[MSFbdMessage alloc] init];
    [fbdMsg setGroupId:mGroupId];
    [fbdMsg setToId:touser1];
    [fbdMsg setFlag:1];
    [fbdMsg setPush:1];
    NSString *outmsgid = [msgMgr sendNotifyForbidden:fbdMsg notifys:arr];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendFbdMsg outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendFbdMsg outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifySettedMgrButton:(id)sender {
    NSString *notifySettedMgr = @"notifySettedMgrButton was called";
    NSLog(@"%@", notifySettedMgr);
    
    mGroupId = @"wocaowocaowocao";
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    
    MSMgrMessage *mgrMsg = [[MSMgrMessage alloc] init];
    [mgrMsg setGroupId:mGroupId];
    [mgrMsg setToId:touser1];
    [mgrMsg setFlag:1];
    [mgrMsg setPush:1];
    NSString *outmsgid = [msgMgr sendNotifySettedMgr:mgrMsg];
    NSString* info = nil;
    if (outmsgid) {
        info = [NSString stringWithFormat:@"sendMgrMsg outmsgid is :%@", outmsgid];
        NSLog(@"%@", info);
    } else {
        info = [NSString stringWithFormat:@"sendMgrMsg outmsgid is :null"];
        NSLog(@"%@", info);
    }
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)settingPushButton:(id)sender {
    NSString *settingPush = @"settingPushButton was called";
    NSLog(@"%@", settingPush);
    
    [clientMgr enablePush:1];
    
    NSString* info = [NSString stringWithFormat:@"settingPush enablePush :%d", 1];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)settingMuteButton:(id)sender {
    NSString *settingMute = @"settingMuteButton was called";
    NSLog(@"%@", settingMute);
    
    [clientMgr muteNotify:1];
    
    NSString* info = [NSString stringWithFormat:@"settingMute muteNotification :%d", 1];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

-(void)updateResult:(NSString*)backInfo
               mMsg:(NSString*)mMsg
               nInt:(int)nInt
{
    NSString *s = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: backInfo:%@,mMsg:%@,code:%i", backInfo, mMsg, nInt]];
    NSLog(@"NSLog:%@", s);
    textViewDisplay.text = s;
}

-(void)resultDisplayCallback:(NSString*)backInfo
                        mMsg:(NSString*)mMsg
                        nInt:(int)nInt
{
    NSMethodSignature *sig = [self methodSignatureForSelector:@selector(updateResult:mMsg:nInt:)];
    if (!sig) {
        return;
    }
    NSInvocation* invo = [NSInvocation invocationWithMethodSignature:sig];
    [invo setTarget:self];
    [invo setSelector:@selector(updateResult:mMsg:nInt:)];
    [invo setArgument:&backInfo atIndex:2];
    [invo setArgument:&mMsg atIndex:3];
    [invo setArgument:&nInt atIndex:4];
    [invo retainArguments];
    [invo performSelectorOnMainThread:@selector(invoke) withObject:nil waitUntilDone:TRUE];
}

- (void)dealloc {
    [clientMgr delDelegateId:impl];
    [groupMgr delDelegateId:impl];
    [msgMgr delDelegateId:impl];
    [clientMgr uninMsgClient];
}
@end
