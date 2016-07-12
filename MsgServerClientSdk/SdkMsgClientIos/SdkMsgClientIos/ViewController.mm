//
//  ViewController.m
//  SdkMsgClientIos
//
//  Created by hp on 1/31/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import <AdSupport/ASIdentifierManager.h>
#import "ViewController.h"
#import "msgclientall/MSClientManager.h"
#import "msgclientall/MSGroupManager.h"
#import "msgclientall/MSMessageManager.h"

@interface ViewController () {
    UIButton *mEnterButton;
    UIButton *mSndMsgButton;
    UIButton *mLeaveButton;
    UIButton *mNotifyButton;
    UITextView *textViewDisplay;
    NSString *mGroupId;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    mEnterButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 100, 200, 30)];
    [mEnterButton setTitle:@"Enter" forState:UIControlStateNormal];
    [mEnterButton setTitle:@"Entering..." forState:UIControlStateHighlighted];
    [mEnterButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mEnterButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mEnterButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mEnterButton.backgroundColor = [UIColor redColor];
    [mEnterButton addTarget:self action:@selector(enterButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mEnterButton];
    
    mSndMsgButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 150, 200, 30)];
    [mSndMsgButton setTitle:@"SndMsg" forState:UIControlStateNormal];
    [mSndMsgButton setTitle:@"SndMsging..." forState:UIControlStateHighlighted];
    [mSndMsgButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mSndMsgButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mSndMsgButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mSndMsgButton.backgroundColor = [UIColor redColor];
    [mSndMsgButton addTarget:self action:@selector(sndMsgButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mSndMsgButton];
    
    mLeaveButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 250, 200, 30)];
    [mLeaveButton setTitle:@"Leave" forState:UIControlStateNormal];
    [mLeaveButton setTitle:@"Leaving..." forState:UIControlStateHighlighted];
    [mLeaveButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mLeaveButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mLeaveButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mLeaveButton.backgroundColor = [UIColor redColor];
    [mLeaveButton addTarget:self action:@selector(leaveButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mLeaveButton];
    
    mNotifyButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 300, 200, 30)];
    [mNotifyButton setTitle:@"Notify" forState:UIControlStateNormal];
    [mNotifyButton setTitle:@"Notifying..." forState:UIControlStateHighlighted];
    [mNotifyButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mNotifyButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mNotifyButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mNotifyButton.backgroundColor = [UIColor redColor];
    [mNotifyButton addTarget:self action:@selector(notifyButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mNotifyButton];
    
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
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)enterButton:(id)sender {
    NSString *enter = @"enterButton was called";
    NSLog(@"%@", enter);
    
    //NSString *groupid = @"4008696859";
    mGroupId = @"wocaowocaowocao";
    NSLog(@"====>>you enter mGroupId %@", mGroupId);
    [groupMgr addGroupGrpId:mGroupId];
}

- (IBAction)sndMsgButton:(id)sender {
    NSString *sndMsg = @"sndMsgButton was called";
    NSLog(@"%@", sndMsg);
    
    NSString *touser = [NSString stringWithCString:"BCD9D958-985A-4454-B2C8-1551DB9C1A8A" encoding:NSUTF8StringEncoding];
    NSString *outmsgid = nil;
    [msgMgr sendTxtMsgToUsrId:touser cont:sndMsg cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendTxtMsgToUsrId outmsgid is :%@", outmsgid];
    NSLog(@"%@", info);
    [self resultDisplayCallback:info mMsg:@"" nInt:0];
}

- (void)leaveButton:(id)sender {
    NSString *leave = @"leaveButton was called";
    NSLog(@"%@", leave);
    
    mGroupId = @"wocaowocaowocao";
    NSLog(@"====>>you leave mGroupId %@", mGroupId);
    [groupMgr rmvGroupGrpId:mGroupId];
}

- (IBAction)notifyButton:(id)sender {
    NSString *notify = @"notifyButton was called";
    NSLog(@"%@", notify);
    
    NSString *outmsgid = nil;
    mGroupId = @"wocaowocaowocao";
    [msgMgr sendTxtMsgGrpId:mGroupId cont:notify cmsgid:&outmsgid];
    NSString *info = [NSString stringWithFormat:@"sendTxtMsgGrpId outmsgid is :%@", outmsgid];
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
