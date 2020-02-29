//
//  ViewController.m
//  client-netec
//
//  Created by zhangxinming on 16/6/14.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#import "ViewController.h"
#import <VoiceTalk/AiYouVoiceManage.h>
#include <string.h> 


typedef enum {
    CS_CONNECTING=0,		//正在连接
    CS_FAILED,				//无法连接
    CS_CONNECTED,			//已经连接
    CS_DISCONNECTED,		//断开连接
    CS_BUSY,				//网络忙(已断开正重连)
    CS_RECONNECTED,			//重连成功
    CS_IDLE,				//空闲
    CS_RESTARTED,			//连接重置了【连接断开了，并且又重新连接上了，但是换了一个新连接】
    CS_LOGINED,             //登陆服务器成功
    CS_LOGINFAILED,         //登陆服务器失败
} CONNECT_STATUS;


typedef struct _CLIENTUSERINFOLIST
{
    _CLIENTUSERINFOLIST():strUserName("")
    ,ulUserId(0)
    ,ulUserVideoID(0)
    ,uiVideoCanSee(0)
    {}
    std::string   strUserName;
    unsigned long ulUserId;
    unsigned long ulUserVideoID;
    unsigned int  uiVideoCanSee;
    std::string   strLocalIP;
    std::string   strNATIP;
}CLIENTUSERINFOLIST;

typedef std::map<unsigned long, CLIENTUSERINFOLIST> CLIENTUSERINFOLIST_MAP;

class INetWorkCallback
{
public:
    virtual void IConnectStatusCallback(CONNECT_STATUS cs) = 0;
    virtual void INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList) = 0;
    virtual void INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName) = 0;
    virtual void INetReceiveUserLogOut(unsigned long uPeerUserID) = 0;
    virtual void INetReceiveData(unsigned long uPeerUserID, const char* pData, unsigned long nLen) = 0;
    virtual void INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen) = 0;
    virtual void INetAudioStatus(unsigned long uPeerUserID,AUDIO_SEND_STATUS AudioStatus) = 0;
    
};


class INetWork
:public INetWorkCallback
{
public:
    ViewController *m_viewController;
    virtual void IConnectStatusCallback(CONNECT_STATUS cs);
    virtual void INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList);
    virtual void INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName);
    virtual void INetReceiveUserLogOut(unsigned long uPeerUserID);
    virtual void INetReceiveData(unsigned long uPeerUserID, const char* pData, unsigned long nLen);
    virtual void INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen);
    virtual void INetAudioStatus(unsigned long uPeerUserID,AUDIO_SEND_STATUS AudioStatus) ;
};

void INetWork::IConnectStatusCallback(CONNECT_STATUS cs)
{
    if (m_viewController) {
            dispatch_async(dispatch_get_main_queue(), ^{
        [m_viewController IConnectStatusCallback:cs];
            });
    }
    
}

void INetWork::INetAudioStatus(unsigned long uPeerUserID, AUDIO_SEND_STATUS AudioStatus)
{
    if (m_viewController) {
        dispatch_async(dispatch_get_main_queue(), ^{
            [m_viewController INetAudioStatus:uPeerUserID AudioStatus:AudioStatus];
          
        });
    }
    
}


void INetWork::INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList)
{
    
    if (m_viewController) {
           dispatch_async(dispatch_get_main_queue(), ^{
        [m_viewController INetReceiveUserList:UserInfoList];
           });
    }
}

void INetWork::INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName)
{
    if (m_viewController) {
           dispatch_async(dispatch_get_main_queue(), ^{
        [m_viewController INetReceiveUserLogin:uPeerUserID strName:strName];
                    });
}
    
}

void INetWork::INetReceiveUserLogOut(unsigned long uPeerUserID)
{
    
    if (m_viewController) {
        [m_viewController INetReceiveUserLogOut:uPeerUserID];
    }
    
}

void INetWork::INetReceiveData(unsigned long uPeerUserID, const char* pData, unsigned long nLen)
{
    
    if (m_viewController) {
                   dispatch_async(dispatch_get_main_queue(), ^{
        [m_viewController INetReceiveData:uPeerUserID pData:pData nLen:nLen];
                   });
    }
}
void INetWork::INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen)
{
    if (m_viewController) {
                   dispatch_async(dispatch_get_main_queue(), ^{
        [m_viewController INetBroadcastData:uPeerUserID pData:pData nLen:nLen];
                   });
    }
    
}

@interface ViewController ()
{
 
   bool m_IsLoginSever;
}
@property (nonatomic,strong)IBOutlet UITextField  *abusername;
@property (nonatomic,strong)IBOutlet UITextField  *password;
@property (nonatomic,strong)IBOutlet UILabel  *loginMsg;
@property (nonatomic,strong)IBOutlet UITextField  *serverip;
@property (nonatomic,strong)IBOutlet UITextField  *userid;
@property (nonatomic,strong)IBOutlet UITextField  *testmessage;
@property (nonatomic,strong)IBOutlet UILabel  *strrotemessage;
@property (nonatomic,strong)IBOutlet UILabel  *videoid;
@property (nonatomic,strong)IBOutlet UIButton  *btvoice;
@end

@implementation ViewController

-(void)btbackimage
{
    [self.abusername resignFirstResponder];
    
    [self.password resignFirstResponder];
    
    [self.serverip resignFirstResponder];
    
    [self.userid resignFirstResponder];
}
-(void)execute:(id)ser
{
                dispatch_async(dispatch_get_main_queue(), ^{
                       [self.loginMsg setText:@"登录成功！"];
                  });
    
}


- (void)applicationDidBecomeActive:(UIApplication *)application {
  
//    loginVoiceServer([self.serverip.text UTF8String],[self.abusername.text UTF8String]);

}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    
    logoutServer();
    
}



- (void)viewDidLoad {
    [super viewDidLoad];
    
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationDidBecomeActive:)
                                                 name:UIApplicationDidBecomeActiveNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(applicationDidEnterBackground:)
                                                 name:UIApplicationDidEnterBackgroundNotification object:nil];
    m_IsLoginSever = NO;
    // Do any additional setup after loading the view, typically from a nib.
     [[NSNotificationCenter defaultCenter] addObserver:self
                                              selector:@selector(execute:)
                                                     name:@"LOGININ"
                                                    object:nil];
   [UIApplication sharedApplication].idleTimerDisabled=YES;
    
//    INetWork *pnetwork=new INetWork;
//      pnetwork->m_viewController=self;
//     m_AppManage = new CAppManage(pnetwork);
 
  UITapGestureRecognizer *singleTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(btbackimage)];
    [    self.view addGestureRecognizer:singleTap];
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)btlongin:(id)sender
{
loginVoiceServer([self.serverip.text UTF8String],[self.abusername.text UTF8String]);
    
 
//    AddRoomUser("t1");
//    AddRoomUser("t2");
//    AddRoomUser("t3");
//    AddRoomUser("t4");
//    AddRoomUser("t5");
//    AddRoomUser("t6");
//    AddRoomUser("t7");
//    AddRoomUser("t8");
//    AddRoomUser("t9");
//    AddRoomUser("t10");
    for(int i=0;i<50;i++)
    {
        NSString *user=[NSString stringWithFormat:@"t%d",i];
        AddRoomUser([user UTF8String]);
        usleep(5000);
    }
//    if (!m_IsLoginSever)
//    {
//      m_AppManage->ConnectServer([self.serverip.text UTF8String]);
//      m_IsLoginSever = true;
//    }
//    else
//    { 
//        m_AppManage->Disconnect();
//        m_IsLoginSever = false;
//    }
    
}
- (IBAction)btlogout:(id)sender
{
    logoutServer();
}


- (IBAction)btsend:(id)sender
{
 
 startSendAudio();
    
}

- (IBAction)btsendUP:(id)sender
{
 stopSendAudio();
}

- (IBAction)btrecive:(id)sender
{
    StartReceiveMedia();

//
//    RtmpStartCameraLive("rtmp://192.168.113.184/live/teststream2");
}


//- (void) INetAudioStatus:(unsigned long )uPeerUserID AudioStatus:(AUDIO_SEND_STATUS )AudioStatus
//{
//    if(AudioStatus == AUDIO_SEND_ENABLE)
//    {
//        [self.btvoice setEnabled:YES];
//    }
//    else
//    {
//       [self.btvoice setEnabled:NO];
//    }
//}
//
//
//- (void) IConnectStatusCallback:(CONNECT_STATUS) cs
//{
//
//    switch (cs)
//    {
//        case CS_CONNECTING:
//            [self.loginMsg setText:@"正在连接服务器。。。"];
//            break;
//        case CS_FAILED:
//                [self.loginMsg setText:@"连接失败！"];
//            break;
//        case CS_DISCONNECTED:
//            [self.loginMsg setText:@"断开连接！"];
//            break;
//        case CS_CONNECTED:
//        {
//           [self.loginMsg setText:@"已经连接！"];
//            if (m_AppManage != NULL)
//            {
//                m_AppManage->LoginServer( [self.abusername.text UTF8String] ,[self.password.text UTF8String]);
//                
//            }
//        }
//            break;
//        case CS_BUSY:
//            [self.loginMsg setText:@"服务器忙！"];
//            
//            break;
//        case CS_RECONNECTED:
//            [self.loginMsg setText:@"重新连接！"];
//            
//            break;
//        case CS_RESTARTED:
//             [self.loginMsg setText:@"重新连接！"];
//            break;
//        case CS_LOGINED:
//        {
//            if (m_AppManage != NULL)
//            {
//                
//        
//                usleep(500);
//                
//               m_AppManage->GetUserList();
//                
//            }
//            
//                    [self.loginMsg setText:@"登录 成功！"];
//        }
//            break;
//        case CS_LOGINFAILED:
//                  [self.loginMsg setText:@"登录失败！"];
//            break;
//        default:
//            break;
//    }
//}
//
//- (void) INetReceiveUserList:(CLIENTUSERINFOLIST_MAP&) UserInfoList
//{
////    m_pUserInfoList = UserInfoList;
////    for(CLIENTUSERINFOLIST_MAP::iterator iter=UserInfoList.begin(); iter!=UserInfoList.end(); iter++)
////    {
////        char strname[255] = {0};
////        sprintf(strname,"%s",iter->second.strUserName.c_str());
////        
////        m_AppManage->m_pMediaManage->OpenRemoteUser(iter->second);
////        
////    }
//    
//}
//
//- (void)INetReceiveUserLogin:(unsigned long )uPeerUserID  strName:(std::string) strName
//{
//    UserInfo mUserInfo;
//    mUserInfo.strUserName = strName;
//    mUserInfo.uUserID = uPeerUserID;
//    CLIENTUSERINFOLIST userinfot;
//    
//    userinfot.ulUserId = uPeerUserID;
//   
////   m_AppManage->m_videoID= XGetTimestamp();
////    
////    userinfot.ulUserVideoID =   m_AppManage->m_videoID;
//    
////     m_AppManage->m_pMediaManage->OpenRemoteUser(userinfot);
//    
////    m_pUserInfoList[uPeerUserID] = mUserInfo;
////    UpDateTreeCtr();
//}
//
//- (void)INetReceiveUserLogOut:(unsigned long )uPeerUserID
//{
//    CLIENTUSERINFOLIST_MAP::iterator iter = m_pUserInfoList.find(uPeerUserID);
//    if(iter != m_pUserInfoList.end())
//    {
//        m_pUserInfoList.erase(iter);
//    }
////    UpDateTreeCtr();
//}
//
//- (void)INetReceiveData:(unsigned long) uPeerUserID  pData:(const char*) pData nLen:(unsigned long )nLen
//{
//     NSString *strmsg=[NSString stringWithCString:pData  encoding:NSUTF8StringEncoding];
//    
////    printf("INetReceiveData uPeerUserID = %lu:%@", uPeerUserID, strmsg);
////    
//    NSString * msg=[NSString stringWithFormat:@"消息：%lu:%@",uPeerUserID,strmsg];
//    
//    [self.strrotemessage setText:msg];
//    
//}
//
//- (void)INetBroadcastData:(unsigned long) uPeerUserID pData:( const char*) pData nLen:(unsigned long )nLen
//{
////    printf("INetBroadcastData uPeerUserID = %d pData = %s \n", uPeerUserID, pData);
//    NSString *strmsg=[NSString stringWithCString:pData  encoding:NSUTF8StringEncoding];
//    
//    NSString * msg=[NSString stringWithFormat:@"广播消息：%lu:%@",uPeerUserID,strmsg];
//    
//    [self.strrotemessage setText:msg];
//}
@end
