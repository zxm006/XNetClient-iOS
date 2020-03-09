//
//  ViewController.m
//  client-XNet
//
//  Created by zhangxinming on 16/6/14.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#import "ViewController.h"
#import <StreamMedia/StreamMedia.h>
#import "UserListCell.h"
#import <AVFoundation/AVFoundation.h>

@implementation UserInfo

@end

@interface ViewController ()<UITableViewDelegate,UITableViewDataSource,UIGestureRecognizerDelegate,VideoChatDelegate,MicBlowDelegate>
{
}

@property (nonatomic,strong)NSMutableArray *m_userList;
@property (nonatomic,assign)NSInteger index;
@property (nonatomic,weak)IBOutlet UITableView *m_contactListView;
@property (nonatomic,weak)IBOutlet UITextField  *abusername;
@property (nonatomic,weak)IBOutlet UITextField  *loginMsgtf;
@property (nonatomic,weak)IBOutlet UITextField  *serverip;
@property (nonatomic,weak)IBOutlet UITextField  *RoomId;
@property (nonatomic,weak)IBOutlet UIButton  *btlogin;
@end

@implementation ViewController

- (void)VideoChatJoin:(CLIENTUSERINFOLIST &)userinfo{
    
}
- (void) micBlowAudiolevel:(float) level
{
    NSLog(@"%@",[NSString stringWithFormat:@"%f", level]);
}

-(void)btbackimage
{
    [self.abusername resignFirstResponder];
    [self.serverip resignFirstResponder];
    [self.RoomId resignFirstResponder];
}

- (IBAction) joinMtg:(id)sender
{
    joinVideoChat([self.RoomId.text UTF8String],YES);
}

- (IBAction) exitMtg:(id)sender
{
    exitVideoChat();
}

- (IBAction)btopencam:(id)sender
{
    openVideoChatLocalVideo();
}

- (IBAction)btclosecam:(id)sender
{
    closeVideoChatLocalVideo();
}

#pragma mark - UIGestureRecognizerDelegate
- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    // 输出点击的view的类名
    NSLog(@"%@", NSStringFromClass([touch.view class]));
    
    // 若为UITableViewCellContentView（即点击了tableViewCell），则不截获Touch事件
    if ([NSStringFromClass([touch.view class]) isEqualToString:@"UITableViewCellContentView"]) {
        return NO;
    }
    return  YES;
}

//通知中心的回调方法

- (void)receiveNotificiation:(NSNotification*)notification{
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
        TouchMoveView  *object=[notification object];
        if (object) {
            //             NSLog(@"%@",object.userName);
            //            CGRect frame = object.frame;
            //            frame.size.width = frame.size.width*2;
            //            frame.size.height = frame.size.height*2;
            //            [object setFrame:frame];
            //            [object setBackgroundColor:[UIColor greenColor]];
            //            object.layer.cornerRadius = object.frame.size.width / 2;
            //                  object.clipsToBounds = YES;
            //             resetLocalVideo();
        }
    });
}

- (void)reOrientation{
    
    //    resetPreview();
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(reOrientation)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:nil];
    
    NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
    
    [center addObserver:self selector:@selector(receiveNotificiation:) name:@"CreateTouchMoveView" object:nil];
    
    _m_userList =[[NSMutableArray alloc]init];
    [UIApplication sharedApplication].idleTimerDisabled=YES;
    UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(btbackimage)];
    
    singleTap.delegate=self;
    
    [self.view addGestureRecognizer:singleTap];
    
    self.m_contactListView.delegate = self;
    self.m_contactListView.dataSource = self;
    self.m_contactListView.separatorStyle = UITableViewCellSeparatorStyleNone;
    [self.m_contactListView registerNib:[UINib nibWithNibName:@"UserListCell" bundle:nil]   forCellReuseIdentifier:@"UserListCellid"];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)MtgInvite:(std::string) username  mtgId:(std::string) mtgId
{
    NSLog(@"%s邀请你加入视频会议 %s",username.c_str(),mtgId.c_str());
}

- (IBAction)btlongin:(id)sender
{
    setVideoChatDelegate(self);
    setVideoChatSession();
    loginVideoChatServer([self.serverip.text UTF8String],[self.abusername.text UTF8String],"nickname","headurl",22.123456789012345,110.123456789012345,[self.RoomId.text UTF8String],false);
    setVideoChatBeautifyFace(false);
}

- (IBAction)btlogout:(id)sender
{
    logoutVideoChatServer();
}

- (void) IConnectStatusCallback:(CONNECT_NET_STATUS) cs
{
    NSLog( @"IConnectStatusCallback");
    [self.btlogin setTitle:@"登录" forState:normal];
    switch (cs)
    {
        case CS_CONNECTING:
            [self.loginMsgtf setText:@"正在连接"];
            break;
        case CS_FAILED:
            [self.loginMsgtf setText:@"连接失败！"];
            break;
        case CS_DISCONNECTED:
            [self.loginMsgtf setText:@"断开连接！"];
            break;
        case CS_CONNECTED:
        {
            [self.loginMsgtf setText:@"已经连接！"];
            
        }
            break;
        case CS_BUSY:
            [self.loginMsgtf setText:@"服务器忙！"];
            
            break;
        case CS_RECONNECTED:
            [self.loginMsgtf setText:@"重新连接服务器！"];
            
            break;
        case CS_RESTARTED:
            [self.loginMsgtf setText:@"已重新连接！"];
            break;
        case CS_LOGINED:
        {
            [self.loginMsgtf setText:@"登录 成功！"];
            [_btlogin setTitle:@"已登录" forState:normal];
        }
            break;
        case CS_LOGINFAILED:
            [self.loginMsgtf setText:@"登录失败！"];
            break;
        case CS_LOGOUT:
            [self.loginMsgtf setText:@"退出成功！"];
            break;
        case CS_RELOGIN:
            [self.loginMsgtf setText:@"您的账号在其他地方登录了！"];
            break;
            
        default:
            break;
    }
}

- (void) INetReceiveUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList
{
    [_m_userList removeAllObjects];
    for(CLIENTUSERINFOLIST_MAP::iterator iter=UserInfoList.begin(); iter!=UserInfoList.end(); iter++)
    {
        char strname[255] = {0};
        sprintf(strname,"%s",iter->second.strUserName.c_str());
        UserInfo *userinfo =[[UserInfo alloc]init];
        userinfo.strUserName=[NSString stringWithCString:iter->second.strUserName.c_str()  encoding:NSUTF8StringEncoding];
        //        userinfo.strUserName =[NSString stringWithFormat:@"%s",iter->second.strUserName.c_str()];
        userinfo.ulUserId =iter->second.ulUserId;
        userinfo.ulUserAudioID =iter->second.ulUserAudioID;
        [_m_userList addObject:userinfo];
    }
    [self.m_contactListView reloadData];
    
}

- (void)INetReceiveUserLogin:(unsigned long )uPeerUserID  strName:(std::string) strName useraudioID:(unsigned long) useraudioID
{
    UserInfo *userinfo =[[UserInfo alloc]init];
    userinfo.strUserName=[NSString stringWithCString:strName.c_str()  encoding:NSUTF8StringEncoding];
    //    userinfo.strUserName =[NSString stringWithFormat:@"%s",strName.c_str()];
    userinfo.ulUserId = uPeerUserID;
    userinfo.ulUserAudioID = useraudioID;
    [_m_userList addObject:userinfo];
    [self.m_contactListView reloadData];
}

- (void)INetReceiveUserLogOut:(unsigned long )uPeerUserID 
{
    for(UserInfo *userinfo in _m_userList)
    {
        if (userinfo.ulUserId == uPeerUserID)
        {
            [_m_userList removeObject:userinfo];
            break;
        }
    }
    [self.m_contactListView reloadData];
}

- (void)INetReceiveData:(unsigned long) uPeerUserID strName:(std::string) strName   strData:(std::string) strData nLen:(unsigned long )nLen
{
    dispatch_async(dispatch_get_main_queue(), ^{
        
//        NSString *userName=[NSString stringWithCString:strName.c_str()  encoding:NSUTF8StringEncoding];
        
        
//        NSString *msg=[NSString stringWithCString:strData.c_str()  encoding:NSUTF8StringEncoding];
        
//        NSString * tmsg=[NSString stringWithFormat:@"%@说:%@",userName,msg];
//        if(msg)
//            [self.loginMsg setText:tmsg];
    });
}

- (void)INetBroadcastData:(unsigned long) uPeerUserID pData:( const char*) pData nLen:(unsigned long )nLen
{
    dispatch_async(dispatch_get_main_queue(), ^{
        
//        NSString *strmsg=[NSString stringWithCString:pData  encoding:NSUTF8StringEncoding];
        
//        NSString * msg=[NSString stringWithFormat:@"广播消息：%lu:%@",uPeerUserID,strmsg];
        
//        [self.loginMsg setText:msg];
    });
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView
 numberOfRowsInSection:(NSInteger)section
{
    return [_m_userList count];
}


UIView * creatVideoWindow(int x,int y,int w,int h)
{
    __block TouchMoveView *touchMoveView =nil;
    dispatch_sync(dispatch_get_main_queue(), ^{
        CGRect frame = CGRectMake(x, y, w, h);
        UIViewController *rootvc = nil;
        UIWindow * window = [[UIApplication sharedApplication] keyWindow];
        if (window.windowLevel != UIWindowLevelNormal) {
            NSArray *windows = [[UIApplication sharedApplication] windows];
            for(UIWindow * tmpWin in windows) {
                if (tmpWin.windowLevel == UIWindowLevelNormal) {
                    window = tmpWin;
                    break;
                }
            }
        }
        rootvc = window.rootViewController;
        touchMoveView= [[TouchMoveView alloc]initWithFrame:frame ];
        touchMoveView.userInteractionEnabled=YES;
        [touchMoveView setBackgroundColor:[UIColor clearColor]];
        [touchMoveView setHidden:YES];
        [ rootvc.view addSubview:touchMoveView];
    });
    return touchMoveView;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"UserListCellid";
    UserListCell *cell = [tableView
                          dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[UserListCell alloc]
                initWithStyle:UITableViewCellStyleDefault
                reuseIdentifier:CellIdentifier];
    }
    
    UserInfo *usinfo=self.m_userList[indexPath.row];
    [cell.labelName setText:usinfo.strUserName];
    // 重用机制，如果选中的行正好要重用
    if (_index == indexPath.row) {
        cell.accessoryType = UITableViewCellAccessoryCheckmark;
    } else {
        cell.accessoryType = UITableViewCellAccessoryNone;
    }
    
    return cell;
}

-(void)ShowAlert:(NSString *)msg
{
    if (msg == nil) {
        return;
    }
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle: @"提示"
                                                    message:msg
                                                   delegate:nil
                                          cancelButtonTitle: @"确定"
                                          otherButtonTitles:nil, nil];
    [alert show];
}


- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    // 取消前一个选中的，就是单选啦
    NSIndexPath *lastIndex = [NSIndexPath indexPathForRow:_index inSection:0];
    UITableViewCell *lastCell = [tableView cellForRowAtIndexPath:lastIndex];
    lastCell.accessoryType = UITableViewCellAccessoryNone;
    
    // 选中操作
    UITableViewCell *cell = [tableView  cellForRowAtIndexPath:indexPath];
    cell.accessoryType = UITableViewCellAccessoryCheckmark;
    // 保存选中的
    _index = indexPath.row;
    [tableView performSelector:@selector(deselectRowAtIndexPath:animated:) withObject:indexPath afterDelay:.5];
//    UserInfo *usinfo=self.m_userList[_index];
//    [self.phonenumber setText:usinfo.strUserName];
}

- (void) MtgInfoReg:(MTG_CMD) mtgCmd result:(bool) result info:(std::string &)info
{
    NSString *strinfo=[NSString stringWithCString:info.c_str()  encoding:NSUTF8StringEncoding];
    NSLog(@"视频会议回应%@",strinfo);
}

- (void) MtgInfo:(MTG_CMD) mtgCmd username:(std::string) username info:(std::string &)info

{
    NSLog(@"视频会议消息 %s ==》%s",username.c_str(),info.c_str());
}

- (void)  UserMediaStatus:(std::string) strName isvideo:(bool) isvideo isopen:(bool) isopen
{
    if (isvideo) {
        if (isopen) {
            NSLog(@"strName == %s 打开视频",strName.c_str() );
        } else {
            NSLog(@"strName == %s 关闭视频",strName.c_str() );
        }
    }
    else {
        if (isopen) {
            NSLog(@"strName == %s 打开语音",strName.c_str() );
        } else {
            NSLog(@"strName == %s 关闭语音",strName.c_str() );
        }
    }
}

- (void) videoChatSnapshot:(UIImage*) image
{
    [self.imageview setImage:image];
}

- (void) UserVideoView:(UIView*) view  userid:(NSString*)userid
{
    NSLog(@"view == %@",userid);
    
    dispatch_async(dispatch_get_main_queue(), ^{
        
        if ([self.abusername.text isEqualToString:userid]) {
//            self.mlocalview = view;
        }
        [self.view addSubview:view];
        [view setFrame:CGRectMake(0,0,120,160)];
        [view setHidden:NO];
    });
    
}

-(void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"CreateTouchMoveView" object:nil];
}
@end
