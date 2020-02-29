//
//  IntercomManage.c
//  IntercomTalk
//
//  Created by zhangxinming on 16/7/21.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#include "IntercomMgr.h"
#include "CIntercomManage.h"
#include <string.h>
#import <UIKit/UIKit.h>
#import "TouchMoveView.h"
#import <AVFoundation/AVFoundation.h>

//#import "IntercomCommon.h"
//#import "AGIntercomApi.h"


static CIntercomManage   *m_intercomManage    =  NULL;



const  NSDictionary *errcodedic = @{@"10001":@"没有摄像头访问权限",@"10002":@"没有读文件的权限",@"10003":@"没有写文件的权限",@"10004":@"没有使用闪光灯权限",@"10005":@"没有外部存储卡读写权限",@"10006":@"没有使用麦克风权限",@"10008":@"没有访问网络的权限",@"11001":@"文件不存在",@"11002":@"文件格式错误",@"11003":@"文件有破损，无法解析",@"11004":@"存储空间不足，无法写入",@"12001":@"网络请求异常，状态码",@"12002":@"返回数据的格式错误",@"40001":@"不合法的调用凭证",@"40002":@"不合法的grant_type",@"40003":@"不合法的OpenID",@"40004":@"不合法的媒体文件类型",@"40007":@"不合法的media_id",@"40008":@"不合法的message_type",@"40009":@"不合法的图片大小",@"40010":@"不合法的语音大小",@"40011":@"不合法的视频大小",@"40012":@"不合法的缩略图大小",@"40013":@"不合法的AppID",@"40014":@"不合法的access_token",@"40020":@"不合法的url长度",@"41001":@"缺失access_token参数",@"41002":@"缺失appid参数",@"41003":@"缺失refresh_token参数",@"41004":@"缺失secret参数",@"41005":@"缺失二进制媒体文件",@"41006":@"缺失media_id参数",@"50001":@"接口未授权"};


NSMutableDictionary *unityMsgdict=[[NSMutableDictionary alloc]init];


class INetWork
:public INetWorkCallback
{
public:
    virtual void IConnectStatusCallback(CONNECT_NET_STATUS cs);
    virtual void INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList);
    virtual void INetReceiveUserLogin(CLIENTUSERINFOLIST & userinfo);
    virtual void INetReceiveUserLogOut(CLIENTUSERINFOLIST & userinfo);
    virtual void INetReceiveData(unsigned long uPeerUserID, std::string strName, std::string strData, unsigned long nLen);
    virtual void INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen);
    virtual void INetAudioStatus(unsigned long uPeerUserID,bool isRoom,AUDIO_SEND_STATUS AudioStatus,std::string strName) ;
    
    virtual void ILoginTime(unsigned long loginTime) ;
 

    
};

static INetWork * m_INetWork =NULL;

id<IntercomDelegate> m_INetWorkDelegate = nil;


int   IntercomManageCallback (int msgcode,bool issucceed,const char * funtype,const char *retmssage)
{
    NSString *smsg=nil;
    if (issucceed&&msgcode==0)
    {
          NSString *strretmssage = [NSString stringWithCString:retmssage encoding:NSUTF8StringEncoding];
        
         smsg=[NSString stringWithFormat:@"%@",strretmssage];
 
        
    }
    else
    {
        NSString *strmsgcode=[NSString stringWithFormat:@"%d",msgcode];
        NSString *strmsg = [errcodedic objectForKey:strmsgcode];
        if(strmsg)
            smsg=[NSString stringWithFormat:@"{\"code\":\"%@\",\"message\":\"%@\"}", strmsgcode,strmsg];
        else
        {
            smsg=[NSString stringWithFormat:@"{\"code\":\"%@\",\"message\":\"%@\"}",strmsgcode,@"未知错误"];
       }
    }
    NSString *strtomsg=[NSString stringWithFormat:@"{\"callBackFun\":\"%s\",\"retMsg\":%@}",funtype,smsg];
  
    //IntercomLOG(@"%@", strtomsg);
    //sendUnityIntercomSendmsg(issucceed,[strtomsg UTF8String]);
   
    return 0;
}

void INetWork::IConnectStatusCallback(CONNECT_NET_STATUS cs)
{
    
    if(cs == CS_LOGOUT||cs == CS_LOGINFAILED||cs == CS_RELOGIN||cs == CS_DISCONNECTED)
    {
        if (m_intercomManage) {
            
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
                
                m_intercomManage->disconnect();
            });
            
            usleep(500000);
            
            delete  m_intercomManage;
            m_intercomManage = NULL;
        }
        if (m_INetWork) {
            delete m_INetWork;
            m_INetWork =NULL;
        }
    }
        
  if (m_INetWorkDelegate) {
        dispatch_async(dispatch_get_main_queue(), ^{
              [m_INetWorkDelegate IConnectStatusCallback:cs];
        });
    }
    
    NSString *strstus=nil;
    switch (cs)
    {
        case CS_CONNECTING:
            
            strstus = @"正在连接服务器。。。\n";
            
            break;
        case CS_FAILED:
            strstus = @"连接失败！\n";
            break;
        case CS_DISCONNECTED:
            strstus = @"断开连接！\n";
            break;
        case CS_CONNECTED:
        {
            strstus = @"已经连接！\n";
            
        }
            break;
        case CS_BUSY:
            strstus = @"服务器忙！\n";
            
            break;
        case CS_RECONNECTED:
            strstus = @"重新连接服务器！\n";
            
            break;
        case CS_RESTARTED:
            //IntercomLOG(@"已重新连接！\n");
            break;
        case CS_LOGINED:
        {
       
            strstus = @"登录 成功！\n";
        }
            break;
        case CS_LOGINFAILED:
            strstus = @"登录失败！\n";
            break;
        case CS_RELOGIN:
            strstus = @"账号在其他设备登录！\n";
            break;
        default:
                     strstus = @"登录中！\n";
            break;
    }
    
    NSString *stttus = [NSString stringWithFormat:@"{\"status\":\"%d\"}",cs];
 
    IntercomManageCallback(0,YES,"MultiIntercomConnectStatus",[stttus UTF8String]);
 
    
    //IntercomLOG(@"服务器连接状态 = %d %@\n",cs,strstus);
}


void INetWork::INetAudioStatus(unsigned long uPeerUserID, bool isRoom, AUDIO_SEND_STATUS AudioStatus,std::string strName)
{
    
   if (m_INetWorkDelegate && [m_INetWorkDelegate respondsToSelector:@selector(INetAudioStatus:isRoom:AudioStatus:strName:)])
   {
       if (m_INetWorkDelegate) {
           dispatch_async(dispatch_get_main_queue(), ^{
               
           [m_INetWorkDelegate INetAudioStatus:uPeerUserID isRoom:isRoom AudioStatus:AudioStatus strName:strName];
           });
       }
 
    }
}

void INetWork::INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList)
{
    if (m_INetWorkDelegate && [m_INetWorkDelegate respondsToSelector:@selector(INetReceiveUserList:)])
    {
//        dispatch_async(dispatch_get_main_queue(), ^{
        
        [m_INetWorkDelegate INetReceiveUserList:UserInfoList];
//        });
    }
}

void INetWork::INetReceiveUserLogin( CLIENTUSERINFOLIST & userinfo)
{
    
    if (m_INetWorkDelegate &&[m_INetWorkDelegate respondsToSelector:@selector(INetReceiveUserLogin:)]) {
//        dispatch_async(dispatch_get_main_queue(), ^{
        
            [m_INetWorkDelegate INetReceiveUserLogin:userinfo];
//        });
    }
    
}

void INetWork::INetReceiveUserLogOut( CLIENTUSERINFOLIST & userinfo)
{
    if (m_INetWorkDelegate &&[m_INetWorkDelegate respondsToSelector:@selector(INetReceiveUserLogOut:)]) {
//        dispatch_async(dispatch_get_main_queue(), ^{
//
            [m_INetWorkDelegate INetReceiveUserLogOut:userinfo];
//        });
    }
}

void INetWork::INetReceiveData(unsigned long uPeerUserID, std::string strName, std::string strData, unsigned long nLen)
{
    
    if (m_INetWorkDelegate &&[m_INetWorkDelegate respondsToSelector:@selector(INetReceiveData:strName:strData:nLen:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_INetWorkDelegate INetReceiveData:uPeerUserID strName:strName.c_str() strData:strData.c_str() nLen:nLen];
        });
    }
}

void INetWork::INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen)
{
    if (m_INetWorkDelegate &&[m_INetWorkDelegate respondsToSelector:@selector(INetBroadcastData:pData:nLen:)]) {
        [m_INetWorkDelegate INetBroadcastData:uPeerUserID pData:pData  nLen:nLen];
    }
}

void INetWork::ILoginTime(unsigned long loginTime)
{
    if (m_INetWorkDelegate &&[m_INetWorkDelegate respondsToSelector:@selector(LoginTime:)]) {
        [m_INetWorkDelegate LoginTime:loginTime];
    }
}

int checkIntercomIsDomain(const char *pAddr)
{
    if(!pAddr || strlen(pAddr) < 1)
    {
        return -1;
    }
    unsigned long  add = inet_addr(pAddr);
    if (add == INADDR_NONE)
    {
        //printf("");
        return 0;
    }
    else
    {
        return 1;
    }
}

int get_Intercom_ip_by_domain(const char *domain, char *ip)
{
    char **pptr;
    struct hostent *hptr;
    
    hptr = gethostbyname(domain);
    if(NULL == hptr)
    {
        printf("gethostbyname error for host:%s/n", domain);
        return -1;
    }
    
    for(pptr = hptr->h_addr_list ; *pptr != NULL; pptr++)
    {
        if (NULL != inet_ntop(hptr->h_addrtype, *pptr, ip, IP_SIZE) )
        {
            return 0; // 只获取第一个 ip
        }
    }
    
    return -1;
}

void loginIntercomServer(const char* szServerHostURL,const char* szUserName, const char* headUrl,const char* nickName,double latitude,double longitude,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId,BOOL listenInRoom,const char* szexpand,bool isencrypt)
{
    
    char ip[IP_SIZE];
    if (checkIntercomIsDomain(szServerHostURL)==1)
    {
        memcpy(ip, szServerHostURL, IP_SIZE);
    }
    else
    {
        int  ret=  get_Intercom_ip_by_domain(mediaServer_URL, ip);
        
        if (ret!=0)
        {
            return;
            
        }
    }
    
    
    if(m_intercomManage)
    {
        m_intercomManage->logoutServer();
 
        usleep(1500000);
 
    }
    
   m_intercomManage = new CIntercomManage(IntercomManageCallback);
    
    if (m_INetWork) {
        delete m_INetWork;
        m_INetWork =NULL;
    }
    
    m_INetWork = new INetWork;
 
    m_intercomManage->setNetWorkCallback(m_INetWork);
    
    if(m_intercomManage)
    {
        m_intercomManage->loginIntercomServer(ip, szUserName, headUrl, nickName, latitude, longitude, szGameId, szGameServerId, szRoomId, szGroupId,listenInRoom, szexpand, isencrypt);
    }
}

void  logoutIntercomServer()
{
      if(m_intercomManage)
    {
        m_intercomManage->logoutServer();
 
    }
   

}

int SendIntercomDataToUser(unsigned long uPeerUserID ,const char* pData, unsigned long nLen)
{

    if(m_intercomManage)
    {
        m_intercomManage->SendDataToUser(uPeerUserID,pData,nLen);
        
    }
    return 0;
}

void  startIntercomAudio(BOOL isspeakinroom)
{
    if(m_intercomManage)
    {
        m_intercomManage->startSendAudio(isspeakinroom);
        
    }
     usleep(100000);
    
//    [[AGIntercomApi shareInstance] successCallbackWithFuncName:AGI_CallbackFunc_StartSend msg:nil];
}

void  stopIntercomAudio()
{
    if(m_intercomManage)
    {
        m_intercomManage->stopSendAudio();
        
    }
    usleep(100000);
    
//    [[AGIntercomApi shareInstance] successCallbackWithFuncName:AGI_CallbackFunc_StopSend msg:nil];
}

 void setIntercomAudioSession()
{
AVAudioSession* session = [AVAudioSession sharedInstance];
NSError *error = nil;

[session setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:&error];
//
//
//    Float64 preferredSampleRate(16000.0);
//    [session setPreferredSampleRate:preferredSampleRate error:&error];
//
//    [session setMode:AVAudioSessionModeVoiceChat error:&error];
//    NSTimeInterval bufferDuration = .02;
//    [session setPreferredIOBufferDuration:bufferDuration error:&error];
//
//[session setActive:YES error:&error];
}

void  setIntercomDelegate(id<IntercomDelegate> delegate)
{
    
       m_INetWorkDelegate =  delegate;
  
}
