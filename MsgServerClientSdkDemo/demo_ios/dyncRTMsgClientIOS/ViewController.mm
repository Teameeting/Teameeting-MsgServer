//
//  ViewController.m
//  MsgClientIos
//
//  Created by hp on 12/25/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#import "ViewController.h"
#import "TMMsgSender.h"

@interface ViewController () {
    UIButton *mLoginButton;
    UIButton *mCreateButton;
    UIButton *mEnterButton;
    UIButton *mSndMsgButton;
    UIButton *mLeaveButton;
    UIButton *mDestroyButton;
    UIButton *mLogoutButton;
    UITextView *textViewDisplay;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    mLoginButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 50, 200, 30)];
    [mLoginButton setTitle:@"Login" forState:UIControlStateNormal];
    [mLoginButton setTitle:@"Logining..." forState:UIControlStateHighlighted];
    [mLoginButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mLoginButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mLoginButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mLoginButton.backgroundColor = [UIColor redColor];
    [mLoginButton addTarget:self action:@selector(loginButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mLoginButton];
    
    mCreateButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 100, 200, 30)];
    [mCreateButton setTitle:@"Create" forState:UIControlStateNormal];
    [mCreateButton setTitle:@"Creating..." forState:UIControlStateHighlighted];
    [mCreateButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mCreateButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mCreateButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mCreateButton.backgroundColor = [UIColor redColor];
    [mCreateButton addTarget:self action:@selector(createButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mCreateButton];
    
    mEnterButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 150, 200, 30)];
    [mEnterButton setTitle:@"Enter" forState:UIControlStateNormal];
    [mEnterButton setTitle:@"Entering..." forState:UIControlStateHighlighted];
    [mEnterButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mEnterButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mEnterButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mEnterButton.backgroundColor = [UIColor redColor];
    [mEnterButton addTarget:self action:@selector(enterButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mEnterButton];
    
    mSndMsgButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 200, 200, 30)];
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
    
    mDestroyButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 300, 200, 30)];
    [mDestroyButton setTitle:@"Destroy" forState:UIControlStateNormal];
    [mDestroyButton setTitle:@"Destroying..." forState:UIControlStateHighlighted];
    [mDestroyButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mDestroyButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mDestroyButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mDestroyButton.backgroundColor = [UIColor redColor];
    [mDestroyButton addTarget:self action:@selector(destroyButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mDestroyButton];
    
    mLogoutButton = [[UIButton alloc] initWithFrame:CGRectMake(10, 350, 200, 30)];
    [mLogoutButton setTitle:@"Logout" forState:UIControlStateNormal];
    [mLogoutButton setTitle:@"Logouting..." forState:UIControlStateHighlighted];
    [mLogoutButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal];
    [mLogoutButton setTitleColor:[UIColor blueColor] forState:UIControlStateHighlighted];
    [mLogoutButton.titleLabel setFont:[UIFont boldSystemFontOfSize:20]];
    mLogoutButton.backgroundColor = [UIColor redColor];
    [mLogoutButton addTarget:self action:@selector(logoutButton:) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:mLogoutButton];
    
    textViewDisplay = [[UITextView alloc] initWithFrame:CGRectMake(250, 100, 150, 300)];
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
    
     msgsender = [ [TMMsgSender alloc] init];
     server = @"192.168.7.39";
     port   = 9210;
     userid = @"test010";
     pass = @"123456";
     roomid = @"roomdi";
     msg = @"hello world";
     impl = [[MsgClientProtocolImpl alloc] init];
     impl.delegate = self;
     if ([msgsender tMInitMsgProtocol:impl server:server port:port] == -1) {
        NSLog(@"msgsender Init Protocol error, return -1!!!");
     }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)loginButton:(id)sender {
    NSString *login = @"loginButton was called";
    NSLog(@"%@", login);
    [msgsender tMLoginUserid:userid pass:pass];
}

- (IBAction)createButton:(id)sender {
    NSString *create = @"createButton was called";
    NSLog(@"%@", create);
    [msgsender tMCreateRoomUserid:userid pass:pass roomid:roomid];
}

- (IBAction)enterButton:(id)sender {
    NSString *enter = @"enterButton was called";
    NSLog(@"%@", enter);
    [msgsender tMEnterRoomUserid:userid pass:pass roomid:roomid];
}

- (IBAction)sndMsgButton:(id)sender {
    NSString *sndMsg = @"sndMsgButton was called";
    NSLog(@"%@", sndMsg);
    [msgsender tMSndMsgUserid:userid pass:pass roomid:roomid msg:msg];
}

- (IBAction)leaveButton:(id)sender {
    NSString *leave = @"leaveButton was called";
    NSLog(@"%@", leave);
    [msgsender tMLeaveRoomUserid:userid pass:pass roomid:roomid];
}

- (IBAction)destroyButton:(id)sender {
    NSString *destroy = @"destroyButton was called";
    NSLog(@"%@", destroy);
    [msgsender tMDestroyRoomUserid:userid pass:pass roomid:roomid];
}

- (IBAction)logoutButton:(id)sender {
    NSString *logout = @"logoutButton was called";
    NSLog(@"%@", logout);
    [msgsender tMLogoutUserid:userid pass:pass];
}

-(void)updateResult:(NSString*)backInfo
               mMsg:(NSString*)mMsg
               nInt:(int)nInt
{
    NSLog(@"ViewController::updateResult was called...");
    NSString *s = [[NSString alloc] initWithString:[NSString stringWithFormat:@"result: backInfo:%@,mMsg:%@,code:%i", backInfo, mMsg, nInt]];
    NSLog(@"NSLog:%@", s);
    textViewDisplay.text = s;
}

-(void)resultDisplayCallback:(NSString*)backInfo
                        mMsg:(NSString*)mMsg
                        nInt:(int)nInt
{
    NSLog(@"ViewController::resultDisplayCallback was called...");
    
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
    [msgsender tMUint];
}
@end
