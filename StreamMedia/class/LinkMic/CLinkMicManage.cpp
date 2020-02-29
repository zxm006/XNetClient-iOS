//#include "StdAfx.h"
#include "CLinkMicManage.h"
#import <Foundation/Foundation.h>
#import "AudioUnitTool.h"
#import "AudioUnitAecTool.h"
#include "OpenRemoteUser.h"
#include "ConnectServer.h"
#include "AutoLock.h"
#import "TouchMoveView.h"
#import <dispatch/queue.h>

CLinkMicManage::CLinkMicManage()
:m_uUserID(0),
m_strAddress(""),
m_serverip(""),
m_OpenLocalUser(NULL),
m_userGroupId(""),
m_iscapscreen(NO),
m_isAnchor(NO),
m_ishasVideo(NO)
{
    XNetSetting::SetAudioProtocolType(XNetSetting::PT_UDP);
    XNetSetting::SetMCUOnly(1);
    m_pINetWorkCallback=NULL;
  
    m_UserInfoList.clear();
    setupSession();
    m_touchMoveView = nil;
    m_peertouchMoveViews =[[NSMutableArray alloc]init];
}

CLinkMicManage::~CLinkMicManage(void)
{
    if(m_peertouchMoveViews)
    {
        [m_peertouchMoveViews release];
        m_peertouchMoveViews =nil;
    }
}


void CLinkMicManage::setupSession()
{
   NSError *error = nil;
// 
     AVAudioSession* session = [AVAudioSession sharedInstance];
    Float64 preferredSampleRate(16000.0);
    [session setPreferredSampleRate:preferredSampleRate error:&error];
 
    [session setMode:AVAudioSessionModeVoiceChat error:&error];
      [session setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:&error];

    NSTimeInterval bufferDuration = .02;
    [session setPreferredIOBufferDuration:bufferDuration error:&error];
    [session setActive:YES error:&error];
    
}
 
int CLinkMicManage::SendData(KCmdPacketEx& pPacket)
{
    return 0;
}



int CLinkMicManage::ConnectServer(const char* szIP,unsigned int nPort,bool isEncrypt)
{
    m_serverip = szIP;
    return m_ConnectServer.Start(szIP,nPort,isEncrypt,this);
}

void CLinkMicManage::disconnect()
{
    m_ConnectServer.Stop();
}

void CLinkMicManage::On_SessionConnectStatus(CONNECT_NET_STATUS cs)
{
    
    if(cs == CS_LOGINED)
    {
          DLog(@"登录成功");
        this->getUserList();
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINED);
    }
    else if(cs == CS_DISCONNECTED)
    {
        DLog(@"登录失败");
       
        this->closeMediaSender();
        this->StopAllRemoteMedia();
    }
    else if(cs == CS_CONNECTED||cs == CS_RECONNECTED||cs == CS_LOGINFAILED)
    {
        this->LoginServer( m_username.c_str() ,"" );
    }
    
    if(m_pINetWorkCallback)
    {
        m_pINetWorkCallback->IConnectStatusCallback(cs);
    }
}

void CLinkMicManage::setNetWorkCallback(ILinkMicCallback*netWorkCallback)
{
    m_pINetWorkCallback   =netWorkCallback;
}

void CLinkMicManage::OnDispatchCmd(KCmdPacketEx& pPacket)
{
    std::string strCMD = pPacket.GetCMD();
    DLog(@"strCMD ==>%s",strCMD.c_str());
   
    if(strCMD=="LOGINSERVERED")
    {
        m_uUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        unsigned long ulErrorType = pPacket.GetAttrib("ERRORTYPE").AsUnsignedLong();
        if(ulErrorType == 0)
        {
            
            NSString *str=@"";
            NSNotification *notification = [NSNotification notificationWithName:@"LOGININ"
                                                                         object:str];
            [[NSNotificationCenter defaultCenter] postNotification:notification];
            DLog(@"登录成功");
            if (!m_isAnchor)
            {
                this->openMediaSender(m_iscapscreen);
              }
         
            
            this->getUserList();
            
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINED);
        }
        else
        {
              DLog(@"登录失败");
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINFAILED);
        }
    }
    
    else if(strCMD=="REMOTEUSERLOGIN")
    {
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        unsigned long ulUseraudioID = pPacket.GetAttrib("USERAUDIOID").AsUnsignedLong();
        
        CLIENTUSERINFOLIST usrinfo;
        usrinfo.strUserName = strUserName;
        usrinfo.ulUserId = ulUserID;
        usrinfo.ulUserAudioID = ulUseraudioID;
        m_UserInfoList[ulUserID] = usrinfo;
        
        
        if (m_isAnchor) {
            OpenRemoteUser* m_OpenRemoteUser = new OpenRemoteUser(false);
            
            m_OpenRemoteUser->ConnectMediaServer(m_serverip, REMOTE_MCUPort);
            m_OpenRemoteUser->OpenAudioReceive(ulUseraudioID);
            
            if (m_ishasVideo) {
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    
                    TouchMoveView  *touchview =(TouchMoveView*)creatVideoWindow(0,100,80,80);
                    [touchview setHidden:YES];
                    
                    m_OpenRemoteUser->SetVideoWindow(touchview);
                    
                    m_OpenRemoteUser->OpenVideoReceive( usrinfo.ulUserAudioID+200);
                    
                    [m_peertouchMoveViews addObject:touchview];
                    
                    touchview.userName = [NSString stringWithFormat:@"%s",strUserName.c_str()];
                    
                    [[NSNotificationCenter defaultCenter] postNotificationName:@"CreateTouchMoveView" object:touchview userInfo:nil];
                    
                });
                
            }
            
            
            m_pOpenRemoteUser_map[ulUserID] = m_OpenRemoteUser;
        }

        
        if(m_pINetWorkCallback)
        {
            m_pINetWorkCallback->INetReceiveUserLogin(ulUserID, strUserName,ulUseraudioID);
        }
        
    }
    else if(strCMD=="REMOTEUSERQUIT")
    {
        
        KAutoLock lock(m_mKCritSec);
        
        unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        std::string strExpand = pPacket.GetAttrib("EXPAND").AsString();
        
        CLIENTUSERINFOLIST_MAP::iterator iter = m_UserInfoList.find(ulUserID);
        
   
        if(m_pINetWorkCallback)
        {
            m_pINetWorkCallback->INetReceiveUserLogOut(ulUserID);
        }
        if(iter != m_UserInfoList.end())
        {
            m_UserInfoList.erase(iter);
        } 
        
            std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(ulUserID);
            if (it!=m_pOpenRemoteUser_map.end())
            {
                 dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                     
                OpenRemoteUser *openRemoteUser=(*it).second;
                openRemoteUser->CloseAudioReceive();
                openRemoteUser->CloseVideoReceive();
                
                openRemoteUser->ReleaseMediaSever();
                delete openRemoteUser;
                     openRemoteUser =NULL;
                 });
                
                m_pOpenRemoteUser_map.erase(it);
            }
       
        
    }
    
    else if(strCMD=="RELOGIN")
    {
        KAutoLock lock(m_mKCritSec);
        closeMediaSender();
        StopAllRemoteMedia();
        
        if(m_pINetWorkCallback) {
            m_pINetWorkCallback->IConnectStatusCallback(CS_RELOGIN);
        }
    }
    
    else if(strCMD=="UPDATEUSERLIST")
    {
        KAutoLock lock(m_mKCritSec);
        CMD_ITEM_LST lstItems = pPacket.GetItemList();
        for(CMD_ITEM_LST::const_iterator it=lstItems.begin();it!=lstItems.end();it++)
        {
            KCmdItem item((std::string)*it);
            CLIENTUSERINFOLIST usrinfo;
            usrinfo.strUserName = item.GetAttrib("USERNAME").AsString();
            usrinfo.ulUserId =  item.GetAttrib("USERID").AsUnsignedLong();
            usrinfo.ulUserAudioID = item.GetAttrib("USERAUDIOID").AsUnsignedLong();
            usrinfo.ulLoginTime = item.GetAttrib("LOGINTIME").AsUnsignedLong();
            
            
            if (m_isAnchor)
            {
                
            OpenRemoteUser* m_OpenRemoteUser = new OpenRemoteUser(false);
            m_OpenRemoteUser->ConnectMediaServer(m_serverip, REMOTE_MCUPort);
            m_OpenRemoteUser->OpenAudioReceive( usrinfo.ulUserAudioID);
            
                
                if (m_ishasVideo)
                {
                    
            TouchMoveView  *touchview =(TouchMoveView*)creatVideoWindow(0,100,80,80);
 
            touchview.userName = [NSString stringWithFormat:@"%s",usrinfo.strUserName.c_str()];
            
                [touchview setHidden:YES];
                
            [[NSNotificationCenter defaultCenter] postNotificationName:@"CreateTouchMoveView" object:touchview userInfo:nil];
            
                [touchview setHidden:YES];
                
            m_OpenRemoteUser->SetVideoWindow(touchview);
            m_OpenRemoteUser->OpenVideoReceive( usrinfo.ulUserAudioID+200);
            [m_peertouchMoveViews addObject:touchview];
                }
                
            m_pOpenRemoteUser_map[usrinfo.ulUserId] = m_OpenRemoteUser;
            }
            
            m_UserInfoList[usrinfo.ulUserId] = usrinfo;
            
        }
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->INetReceiveUserList(m_UserInfoList);
        
    }
    else if(strCMD=="SIGNALINGTRANSFER")
    {
        bool isbroadcast = pPacket.GetAttrib("BROADCAST").AsBoolean();
        std::string strData = pPacket.GetAttrib("DATA").AsString();
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        printf("strData =%s\n\r",strData.c_str());
        if(isbroadcast)
        {
            unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetBroadcastData(ulUserID, strData.c_str(), strData.size());
        }
        else
        {
            unsigned long ulUserID = pPacket.GetAttrib("PEERUSERID").AsUnsignedLong();
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetReceiveData(ulUserID,strUserName, strData.c_str(), strData.size());
        }
    }
    else if(strCMD=="USERQUIT")
    {
          printf("USERQUIT =====  >USERQUIT");
        
        closeMediaSender();
        StopAllRemoteMedia();
        
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->IConnectStatusCallback(CS_LOGOUT);
   
      
//

    }
}

int CLinkMicManage::LoginServer(const char* szName, const char* szPassword)
{
    if(!szName || !szPassword)
        return -1;
 
    m_username = szName;
    srand((unsigned)time(NULL));
    unsigned int time=[[NSDate date] timeIntervalSince1970]+rand()%1000000;
    this->m_audioID=time-1400000000;
    
    std::string strLogin = "LoginServer";
    KCmdPacketEx rPacket(strLogin.c_str(),(int)strLogin.length()+1);
    std::string strCMD = "LOGINSERVER";
    rPacket.SetCMD(strCMD);
    std::string strname = "USERNAME";
    rPacket.SetAttrib(strname,szName);
    std::string strPass = "USERPASS";
    rPacket.SetAttrib(strPass,szPassword);
    std::string strAddress = "ADDRESS";
    rPacket.SetAttrib(strAddress,m_strAddress);
    std::string straudioID = "USERAUDIOID";
    rPacket.SetAttribUL(straudioID, this->m_audioID);
    std::string groupid = "GROUPID";
    rPacket.SetAttrib(groupid, m_userGroupId);
    std::string expand = "EXPAND";
    rPacket.SetAttrib(expand, m_szexpand);
    return m_ConnectServer.SendData(rPacket);
}

int CLinkMicManage::logoutServer()
{
    [[AudioUnitTool shareHandle]stop];
    
    std::string strinfo = "quitserver";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "QUITSERVER";
    rPacket.SetCMD(strCMD);
    
 
    m_UserInfoList.clear();
   
    return m_ConnectServer.SendData(rPacket);
}

int CLinkMicManage::getUserList()
{
    std::string strGetuserlist= "getuserlist";
    KCmdPacketEx rPacket(strGetuserlist.c_str(),(int)strGetuserlist.length()+1);
    rPacket.SetAttrib("ADDRESS", m_strAddress);
    std::string strCMD = "GETUSERLIST";
    rPacket.SetCMD(strCMD);
    return m_ConnectServer.SendData(rPacket);
}

void CLinkMicManage::loginLinkMicServer(const char* szIP,const char* szName ,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId ,const char* szexpand,bool isencrypt,bool iscapscreen,bool isAnchor,bool ishaveVideo)
{
    if(!szIP||!szName||!szGameId||!szGameServerId||!szRoomId)
    {
          return;
    }
    m_iscapscreen = iscapscreen;
    m_isAnchor = isAnchor;
    m_szexpand = szexpand;
    char sAddress[255];
    sprintf(sAddress,"%s-%s-%s",szGameId,szGameServerId,szRoomId);
    m_strAddress = sAddress;
    m_username = szName;
    m_ishasVideo = ishaveVideo;
    if(!szGroupId)
    {
        m_userGroupId = "";
    }
    else
    {
        m_userGroupId = szGroupId;
    }
     ConnectServer(szIP,REMOTE_MCUPort,isencrypt);
}

int CLinkMicManage::SendLinkMic(const char* pData, unsigned long nLen,bool isinroom )
{
    if(!pData || nLen == 0)
        return -1;
    //不在组里却发组的消息。
    if (!isinroom&&m_userGroupId.compare("")==0)
    {
        return -1;
    }
    
   std::string strinfo = "SENDLinkMic";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "SENDLinkMic";
    rPacket.SetCMD(strCMD);
    rPacket.SetAttribBL("ISINROOM", isinroom);
    rPacket.SetAttrib("DATA", pData);
    rPacket.SetAttribUL("USERID", m_uUserID);
    rPacket.SetAttrib("USERNAME", m_username);
    rPacket.SetAttrib("ADDRESS", m_strAddress);
    rPacket.SetAttrib("GROUPID", m_userGroupId);
    rPacket.SetAttribUL("USERAUDIOID", m_audioID);
    rPacket.SetAttrib("EXPAND", m_szexpand);
    
    return m_ConnectServer.SendData(rPacket);
}

int CLinkMicManage::SendDataToUser(unsigned long uPeerUserID ,const char* pData, unsigned long nLen)
{
    if(!pData || nLen == 0)
        return -1;
    std::string strinfo = "Signalingtransfer";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "SIGNALINGTRANSFER";
    rPacket.SetCMD(strCMD);
    rPacket.SetAttribBL("BROADCAST", false);
    rPacket.SetAttribUL("PEERUSERID", uPeerUserID);
    rPacket.SetAttrib("DATA", pData);
    rPacket.SetAttribUL("USERID", m_uUserID);
    rPacket.SetAttrib("USERNAME", m_username);
    return m_ConnectServer.SendData(rPacket);
}

int CLinkMicManage::SendAllUserData(const char* pData, unsigned long nLen)
{
    if(!pData || nLen == 0)
        return -1;
    
    std::string strinfo = "Signalingtransfer";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "SIGNALINGTRANSFER";
    rPacket.SetCMD(strCMD);
    rPacket.SetAttribBL("BROADCAST", true);
    rPacket.SetAttribUL("USERID", m_uUserID);
    rPacket.SetAttribUL("USERAUDIOID", this->m_audioID);
    rPacket.SetAttrib("DATA", pData);
    return m_ConnectServer.SendData(rPacket);
}

std::string CLinkMicManage::GetLocalIP()
{
    return m_ConnectServer.GetLocalIP();
}

std::string CLinkMicManage::GetNATIP()
{
    return m_ConnectServer.GetNATIP();
}

void CLinkMicManage::SendAudioData(char*pData, int nLen)
{
    if (m_OpenLocalUser )
    {
        m_OpenLocalUser->SendAudioData(pData, nLen);
    }
}



void* CLinkMicManage::creatVideoWindow(int x,int y,int w,int h)
{
    
    __block TouchMoveView *touchMoveView =nil;
    dispatch_async(dispatch_get_main_queue(), ^{
    CGRect frame = CGRectMake(x, y, w, h);
    
    UIViewController *rootvc = nil;
    UIWindow * window = [[UIApplication sharedApplication] keyWindow];
    if (window.windowLevel != UIWindowLevelNormal)
    {
        NSArray *windows = [[UIApplication sharedApplication] windows];
        for(UIWindow * tmpWin in windows)
        {
            if (tmpWin.windowLevel == UIWindowLevelNormal)
            {
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


void CLinkMicManage::resetLocalVideo()
{
    if (m_OpenLocalUser) {
        m_OpenLocalUser->reOrientation();
    }
}

void CLinkMicManage::openMediaSender(bool isCapScreen)
{
    KAutoLock lock(m_mKCritSec);
    if (!m_OpenLocalUser)
    {
        m_OpenLocalUser = new OpenLocalUser;
        m_OpenLocalUser->ConnectMediaServer(m_serverip, LOCAL_MCUPort);
        m_OpenLocalUser->OpenAudioSend((unsigned int)this->m_audioID);
        if (m_ishasVideo)
        {
        m_OpenLocalUser->OpenVideoSend((unsigned int)this->m_audioID+200);
       dispatch_async(dispatch_get_main_queue(), ^{
            
        if (m_touchMoveView) {
            
            [m_touchMoveView removeFromSuperview];
            [m_touchMoveView release];
            m_touchMoveView = nil;
        }
        });
            
        openLocalAudio();
        m_touchMoveView = (TouchMoveView*)creatVideoWindow(0,0,80,80);
 
        [m_touchMoveView setHidden:NO];
        
                m_OpenLocalUser->openLocalVideo(m_touchMoveView,0,isCapScreen);
   
        m_touchMoveView.userName = [NSString stringWithFormat:@"%s",m_username.c_str()];
        
        [[NSNotificationCenter defaultCenter] postNotificationName:@"CreateTouchMoveView" object:m_touchMoveView userInfo:nil];
        }
        
    }
}

void CLinkMicManage::closeMediaSender()
{
//     KAutoLock lock(m_mKCritSec);
    if (m_OpenLocalUser != NULL)
    {
        closeLocalAudio();
        m_OpenLocalUser->CloseAudioSend();
        m_OpenLocalUser->ReleaseMediaSever();
        
        if (m_OpenLocalUser)
        {
       m_OpenLocalUser->closeLocalVideo();
   
       if(m_touchMoveView)
            {
                [m_touchMoveView removeFromSuperview];
                [m_touchMoveView release];
                m_touchMoveView =nil;
            }
    }
     delete m_OpenLocalUser;
        m_OpenLocalUser = NULL;
    }
}

void CLinkMicManage::closeLocalAudio( )
{
        [[AudioUnitTool shareHandle]setSendCmdManage:NULL];
        [[AudioUnitTool shareHandle]stop];
        [AudioUnitTool closeHandle];
}

void CLinkMicManage::openLocalAudio()
{
        [[AudioUnitTool shareHandle]setSendCmdManage:this];
        [[AudioUnitTool shareHandle]start];
}

void CLinkMicManage::StopAllRemoteMedia()
{
    KAutoLock lock(m_mKCritSec);
    std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.begin();
    while (it!=m_pOpenRemoteUser_map.end())
    {
         dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
             
        OpenRemoteUser *openRemoteUser=(*it).second;
        openRemoteUser->CloseAudioReceive();
        
        openRemoteUser->CloseVideoReceive();
             usleep(500);
             
        openRemoteUser->ReleaseMediaSever();
        delete openRemoteUser;
             openRemoteUser = NULL;
         });
        
        m_pOpenRemoteUser_map.erase(it++);
    }
    
    if (m_peertouchMoveViews) {
        
        for(TouchMoveView *moveview in m_peertouchMoveViews)
        {
            if(moveview)
            {
                [moveview removeFromSuperview];
                [moveview release];
                moveview =nil;
            }
        }
        [ m_peertouchMoveViews release];
        m_peertouchMoveViews =nil;
        
        
    }
}

void CLinkMicManage::OpenRemoteUserVideo(const char* username  )
{
    
    std::string strusername = username;
    
    OpenRemoteUser* m_OpenRemoteUser = new OpenRemoteUser(false);
    
    m_OpenRemoteUser->ConnectMediaServer(m_serverip, REMOTE_MCUPort);
    
    CLIENTUSERINFOLIST TMPuserinfo;
    
    CLIENTUSERINFOLIST_MAP::iterator iter =m_UserInfoList.begin();
    while (iter!=m_UserInfoList.end())
    {
        CLIENTUSERINFOLIST userinfo=(*iter).second;
        
        if(userinfo.strUserName  == strusername)
        {
            TouchMoveView  *touchview =(TouchMoveView*)creatVideoWindow(0,100,80,80);
            m_OpenRemoteUser->SetVideoWindow(touchview);
            touchview.userName = [NSString stringWithFormat:@"%s",userinfo.strUserName.c_str()];
            
            [[NSNotificationCenter defaultCenter] postNotificationName:@"CreateTouchMoveView" object:touchview userInfo:nil];
            
            
            m_OpenRemoteUser->OpenVideoReceive( userinfo.ulUserAudioID+200);
            
            
            [m_peertouchMoveViews addObject:touchview];
            
            m_pOpenRemoteUser_map[userinfo.ulUserId] = m_OpenRemoteUser;
            break;
        }
        
          iter++;
    }
}


void CLinkMicManage::closeRemoteUserVideo(const char* username  )
{
    
    std::string strusername = username;
    
    CLIENTUSERINFOLIST_MAP::iterator iter =m_UserInfoList.begin();
    while (iter!=m_UserInfoList.end())
    {
        CLIENTUSERINFOLIST userinfo=(*iter).second;
        
        if(userinfo.strUserName  == strusername)
        {
            
            std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(userinfo.ulUserId);
            if (it!=m_pOpenRemoteUser_map.end())
            {
                dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                    
                OpenRemoteUser *openRemoteUser=(*it).second;
                openRemoteUser->CloseAudioReceive();
                openRemoteUser->CloseVideoReceive();
                    usleep(500);
                openRemoteUser->ReleaseMediaSever();
                delete openRemoteUser;
                    openRemoteUser = NULL;
                    
                });
                
                m_pOpenRemoteUser_map.erase(it);
            }
            
 
            
            break;
        }
        iter++;  
    }
    
}

