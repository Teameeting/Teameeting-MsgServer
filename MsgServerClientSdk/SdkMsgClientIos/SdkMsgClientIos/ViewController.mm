//
//  ViewController.m
//  SdkMsgClientIos
//
//  Created by hp on 1/31/16.
//  Copyright © 2016 Dync. All rights reserved.
//

#import "ViewController.h"
#import "TMMsgSender.h"

@interface ViewController () {
    UIButton *mEnterButton;
    UIButton *mSndMsgButton;
    UIButton *mLeaveButton;
    UIButton *mNotifyButton;
    UITextView *textViewDisplay;
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
    uid = @"9a4f3730-f643-422a-a3a1-eae557060a90";
    token = @"a405f2ad61030c3e354a144137213f819d32516896d7ed883d1dfb05dcd993bd8578d422bbf1e84f5cce15316374a217";
    nname = @"nickname";
    roomid = @"400000000440";
    msg = @"hello world";
    impl = [[MsgClientProtocolImpl alloc] init];
    impl.delegate = self;
    if ([msgsender tMInitMsgProtocol:impl uid:uid token:token nname:nname server:server port:port] == -1) {
        NSLog(@"msgsender Init Protocol error, return -1!!!");
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)enterButton:(id)sender {
    NSString *enter = @"enterButton was called";
    NSLog(@"%@", enter);
    [msgsender tMOptRoomCmd:MCMeetCmdENTER roomid:roomid rname:@"roomname" remain:@""];
}

- (IBAction)sndMsgButton:(id)sender {
    NSString *sndMsg = @"sndMsgButton was called";
    NSLog(@"%@", sndMsg);
    [msgsender tMSndMsgRoomid:roomid rname:@"roomname" msg:msg];
}

- (IBAction)leaveButton:(id)sender {
    NSString *leave = @"leaveButton was called";
    NSLog(@"%@", leave);
    [msgsender tMOptRoomCmd:MCMeetCmdLEAVE roomid:roomid rname:@"roomname" remain:@""];
}

- (IBAction)notifyButton:(id)sender {
    NSString *notify = @"notifyButton was called";
    NSLog(@"%@", notify);
    [msgsender tMNotifyMsgRoomid:roomid rname:@"roomname" tags:MCSendTagsTALK msg:msg];
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
