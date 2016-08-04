//
//  ViewController.m
//  SdkMsgClientIos
//
//  Created by hp on 1/31/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <AdSupport/ASIdentifierManager.h>
#import "ViewController.h"
#import "msgclient/MSClientManager.h"
#import "msgclient/MSGroupManager.h"
#import "msgclient/MSMessageManager.h"

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

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
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
    
    ////////////////////////////////////////////////////
    clientMgr = [[MSClientManager alloc] init];
    groupMgr = [[MSGroupManager alloc] init];
    msgMgr = [[MSMessageManager alloc] init];
    impl = [[MsgClientProtocolImpl alloc] init];
    impl.delegate = self;
    
    
    server = @"192.168.7.207";
    port   = 6630;
    uid = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
    NSLog(@"this device uid is:%@", uid);
    token = @"a405f2ad61030c3e354a144137213f819d32516896d7ed883d1dfb05dcd993bd8578d422bbf1e84f5cce15316374a217";
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
    [clientMgr setToken:@"hehehe"];
    [clientMgr setUIconUrl:@"hehehehe"];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)enterButton:(id)sender {
    NSString *enter = @"enterButton was called";
    NSLog(@"%@", enter);
    
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
    
    NSString *outmsgid = nil;
    mGroupId = @"wocaowocaowocao";
    [msgMgr sendTxtMsgGrpId:mGroupId cont:sndMsg cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendTxtMsgGrpId outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)sndMsgTosButton:(id)sender {
    NSString *sndMsgTos = @"sndMsgTosButton was called";
    NSLog(@"%@", sndMsgTos);
    
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *touser2 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSArray* arr = [NSArray arrayWithObjects:touser1, touser2, nil];
    mGroupId = @"wocaowocaowocao";
    NSString *outmsgid = nil;
    [msgMgr sendTxtMsgTosGrpId:mGroupId users:arr cont:sndMsgTos cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendTxtMsgTos outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)sndMsgToUserButton:(id)sender {
    NSString *sndMsgToUser = @"sndMsgToUserButton was called";
    NSLog(@"%@", sndMsgToUser);
    
    NSString *touser = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *outmsgid = nil;
    [msgMgr sendTxtMsgToUsrId:touser cont:sndMsgToUser cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendTxtMsgToUsrId outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)sndMsgToUsersButton:(id)sender {
    NSString *sndMsgToUsers = @"sndMsgToUsersButton was called";
    NSLog(@"%@", sndMsgToUsers);
    
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *touser2 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSArray *arr = [NSArray arrayWithObjects:touser1, touser2, nil];
    NSString *outmsgid = nil;
    [msgMgr sendTxtMsgToUsrIds:arr cont:sndMsgToUsers cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendTxtMsgToUsrIds outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifyLiveButton:(id)sender {
    NSString *notifyLive = @"notifyLiveButton was called";
    NSLog(@"%@", notifyLive);
    
    NSString *outmsgid = nil;
    mGroupId = @"wocaowocaowocao";
    NSString *touser = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    [msgMgr sendNotifyLiveGrpId:mGroupId hostId:touser cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendNotifyLive outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifyRedEnvelopeButton:(id)sender {
    NSString *notifyRedEnvelope = @"notifyRedEnvelopeButton was called";
    NSLog(@"%@", notifyRedEnvelope);
    
    NSString *outmsgid = nil;
    mGroupId = @"wocaowocaowocao";
    NSString *touser = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *cash = @"123.45";
    NSString *wishcont = @"happy to you";
    [msgMgr sendNotifyRedEnvelopeGrpId:mGroupId hostId:touser cash:cash cont:wishcont cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendRedRevelope outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifyBlacklistButton:(id)sender {
    NSString *notifyBlacklist = @"notifyBlacklistButton was called";
    NSLog(@"%@", notifyBlacklist);
    
    NSString *outmsgid = nil;
    mGroupId = @"wocaowocaowocao";
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *touser2 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSArray *arr = [NSArray arrayWithObjects:touser1, touser2, nil];
    [msgMgr sendNotifyBlacklistGrpId:mGroupId userId:touser1 notifys:arr cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendNotifyBlacklist outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifyForbiddenButton:(id)sender {
    NSString *notifyForbidden = @"notifyForbiddenButton was called";
    NSLog(@"%@", notifyForbidden);
    
    NSString *outmsgid = nil;
    mGroupId = @"wocaowocaowocao";
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *touser2 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSArray *arr = [NSArray arrayWithObjects:touser1, touser2, nil];
    [msgMgr sendNotifyForbiddenGrpId:mGroupId userId:touser1 notifys:arr cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendNotifyForbidden outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (IBAction)notifySettedMgrButton:(id)sender {
    NSString *notifySettedMgr = @"notifySettedMgrButton was called";
    NSLog(@"%@", notifySettedMgr);
    
    NSString *outmsgid = nil;
    mGroupId = @"wocaowocaowocao";
    NSString *touser1 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *touser2 = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    [msgMgr sendNotifySettedMgrGrpId:mGroupId userId:touser1 cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendNotifySettedMgr outmsgid is :%@", outmsgid];
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
